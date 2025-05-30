// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CollabPawnExtensionComponent.h"

#include "CollabGameplayTags.h"
#include "CollabLog.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"
#include "Character/CollabPawnData.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameplayAbilitySystem/CollabAbilitySet.h"
#include "GameplayAbilitySystem/Effects/CollabGameplayEffect.h"
#include "Net/UnrealNetwork.h"


const FName UCollabPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

// Sets default values for this component's properties
UCollabPawnExtensionComponent::UCollabPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void UCollabPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	// Listen for changes to all features
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);
	
	// Notifies state manager that we have spawned, then try rest of default initialization
	ensure(TryToChangeInitState(CollabGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
	
}

void UCollabPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();
	UnregisterInitStateFeature();
	
	Super::EndPlay(EndPlayReason);
}

void UCollabPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("CollabPawnExtensionComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(UCollabPawnExtensionComponent::StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1), TEXT("Only one CollabPawnExtensionComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	// Register with the init state system early, this will only work if this is a game world
	RegisterInitStateFeature();
}

void UCollabPawnExtensionComponent::OnRep_PawnData()
{
	CheckDefaultInitialization();
}

void UCollabPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PawnData);
}


// Called every frame
void UCollabPawnExtensionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCollabPawnExtensionComponent::SetPawnData(const UCollabPawnData* InPawnData)
{
	check(InPawnData);

	APawn* Pawn = GetPawnChecked<APawn>();

	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogCollab, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(Pawn), *GetNameSafe(PawnData));
		return;
	}

	PawnData = InPawnData;

	Pawn->ForceNetUpdate();

	CheckDefaultInitialization();
}

bool UCollabPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	if (!CurrentState.IsValid() && DesiredState == CollabGameplayTags::InitState_Spawned)
	{
		// As long as we are on a valid pawn, we count as spawned
		if (IsValid(Pawn))
		{
			return true;
		}
	}
	if (CurrentState == CollabGameplayTags::InitState_Spawned && DesiredState == CollabGameplayTags::InitState_DataAvailable)
	{
		// Pawn data is required.
		if (!IsValid(PawnData))
		{
			return false;
		}

		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		if (bHasAuthority || bIsLocallyControlled)
		{
			// Check for being possessed by a controller.
			if (!GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == CollabGameplayTags::InitState_DataAvailable && DesiredState == CollabGameplayTags::InitState_DataInitialized)
	{
		// Transition to initialize if all features have their data available
		const bool bFeaturesReady = Manager->HaveAllFeaturesReachedInitState(Pawn, CollabGameplayTags::InitState_DataAvailable);
		return bFeaturesReady;
	}
	else if (CurrentState == CollabGameplayTags::InitState_DataInitialized && DesiredState == CollabGameplayTags::InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UCollabPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (DesiredState == CollabGameplayTags::InitState_DataInitialized)
	{
		// This is currently all handled by other components listening to this state change
	}
}

void UCollabPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// If another feature is now in DataAvailable, see if we should transition to DataInitialized
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		if (Params.FeatureState == CollabGameplayTags::InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

void UCollabPawnExtensionComponent::CheckDefaultInitialization()
{
	// Before checking our progress, try progressing any other features we might depend on
	CheckDefaultInitializationForImplementers();

	static const TArray<FGameplayTag> StateChain = { CollabGameplayTags::InitState_Spawned, CollabGameplayTags::InitState_DataAvailable, CollabGameplayTags::InitState_DataInitialized, CollabGameplayTags::InitState_GameplayReady };

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void UCollabPawnExtensionComponent::InitializeAbilitySystem(UCollabAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);

	if (AbilitySystemComponent == InASC)
	{
		// The ability system component hasn't changed.
		return;
	}

	if (AbilitySystemComponent.Get())
	{
		// Clean up the old ability system component.
		UninitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();

	UE_LOG(LogCollab, Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s] "), *GetNameSafe(InASC), *GetNameSafe(Pawn), *GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));

	if (IsValid(ExistingAvatar) && (ExistingAvatar != Pawn))
	{
		UE_LOG(LogCollab, Log, TEXT("Existing avatar (authority=%d)"), ExistingAvatar->HasAuthority() ? 1 : 0);

		// There is already a pawn acting as the ASC's avatar, so we need to kick it out
		// This can happen on clients if they're lagged: their new pawn is spawned + possessed before the dead one is removed
		// ensure(!ExistingAvatar->HasAuthority());

		if (UCollabPawnExtensionComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);
	if (!HasAuthority())
	{
		UE_LOG(LogCollab, Log, TEXT("Not Authority!"));
	}

	if (HasAuthority())
	{		
		if (IsValid(PawnData))
		{
			for (const FCollabAbilitySet_GameplayEffect& SpawnEffect : PawnData->SpawnEffects)
			{
				UClass* LoadedSpawnEffect = SpawnEffect.GameplayEffect.LoadSynchronous();
				if (!IsValid(LoadedSpawnEffect) || !LoadedSpawnEffect->IsChildOf(UCollabGameplayEffect::StaticClass()))
				{
					continue;
				}

				FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
				EffectContext.AddSourceObject(AbilitySystemComponent->GetAvatarActor());

				FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(LoadedSpawnEffect, SpawnEffect.EffectLevel, EffectContext);
				if (EffectSpecHandle.IsValid())
				{
					FActiveGameplayEffectHandle GameplayEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
				}
			}
		}
	}

	// Broadcast ability system initialized here for death/health component to bind to
	OnAbilitySystemInit.Broadcast(AbilitySystemComponent.Get());
}

void UCollabPawnExtensionComponent::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		FGameplayTagContainer AbilityTypesToIgnore;
		AbilityTypesToIgnore.AddTag(CollabGameplayTags::Ability_Behavior_SurvivesDeath);

		AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
		// AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
			AbilitySystemComponent->ClearActorInfo();
		}

		// OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}

void UCollabPawnExtensionComponent::HandleControllerChanged()
{
	if (AbilitySystemComponent.IsValid() && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	{
		ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());
		if (AbilitySystemComponent->GetOwnerActor() == nullptr)
		{
			UninitializeAbilitySystem();
		}
		else
		{
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
	}
	
	CheckDefaultInitialization();
}

void UCollabPawnExtensionComponent::HandlePlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void UCollabPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}

