// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CollabDeathComponent.h"

#include "CollabGameplayTags.h"
#include "CollabLog.h"
#include "Character/CollabCharacterBase.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UCollabDeathComponent::UCollabDeathComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	// ...
	DeathState = ECollabDeathState::NotDead;
}

void UCollabDeathComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCollabDeathComponent, DeathState);
}

void UCollabDeathComponent::InitializeWithAbilitySystem(UCollabAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent.IsValid())
	{
		UE_LOG(LogCollab, Error, TEXT("CollabHealthComponent: Health component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent.IsValid())
	{
		UE_LOG(LogCollab, Error, TEXT("CollabHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	ClearGameplayTags();
}

void UCollabDeathComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();

	AbilitySystemComponent = nullptr;
}


void UCollabDeathComponent::StartDeath()
{
	if (DeathState != ECollabDeathState::NotDead)
	{
		return;
	}

	DeathState = ECollabDeathState::DeathStarted;

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(CollabGameplayTags::Status_Death_Dying, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathStarted.Broadcast(Owner);

	ACollabCharacterBase* CollabCharacterOwner = Cast<ACollabCharacterBase>(Owner);
	if (IsValid(CollabCharacterOwner))
	{
		CollabCharacterOwner->BeginDestroyCharacter(true);
	}

	Owner->ForceNetUpdate();
}

void UCollabDeathComponent::FinishDeath()
{
	if (DeathState != ECollabDeathState::DeathStarted)
	{
		return;
	}

	DeathState = ECollabDeathState::DeathFinished;

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(CollabGameplayTags::Status_Death_Dead, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner);

	ACollabCharacterBase* CollabCharacterOwner = Cast<ACollabCharacterBase>(Owner);
	if (IsValid(CollabCharacterOwner))
	{
		CollabCharacterOwner->FinishDestroyCharacter();
	}

	Owner->ForceNetUpdate();
}

// Called when the game starts
void UCollabDeathComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UCollabDeathComponent::OnRegister()
{
	Super::OnRegister();

	AActor* Owner = GetOwner();
	check(Owner);

	ACollabCharacterBase* CollabCharacterOwner = Cast<ACollabCharacterBase>(Owner);
	if (ensure(IsValid(CollabCharacterOwner)))
	{
		CollabCharacterOwner->OnAbilitySystemInit.AddUniqueDynamic(this, &ThisClass::InitializeWithAbilitySystem);
	}
}


// Called every frame
void UCollabDeathComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCollabDeathComponent::OnRep_DeathState(ECollabDeathState OldDeathState)
{
	const ECollabDeathState NewDeathState = DeathState;

	// Revert the death state for now since we rely on StartDeath and FinishDeath to change it.
	DeathState = OldDeathState;

	if (OldDeathState > NewDeathState)
	{
		// The server is trying to set us back but we've already predicted past the server state.
		UE_LOG(LogCollab, Warning, TEXT("CollabHealthComponent: Predicted past server death state [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		return;
	}

	if (OldDeathState == ECollabDeathState::NotDead)
	{
		if (NewDeathState == ECollabDeathState::DeathStarted)
		{
			StartDeath();
		}
		else if (NewDeathState == ECollabDeathState::DeathFinished)
		{
			StartDeath();
			FinishDeath();
		}
		else
		{
			UE_LOG(LogCollab, Error, TEXT("CollabHealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}
	}
	else if (OldDeathState == ECollabDeathState::DeathStarted)
	{
		if (NewDeathState == ECollabDeathState::DeathFinished)
		{
			FinishDeath();
		}
		else
		{
			UE_LOG(LogCollab, Error, TEXT("CollabHealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}
	}

	ensureMsgf((DeathState == NewDeathState), TEXT("CollabHealthComponent: Death transition failed [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
}

void UCollabDeathComponent::ClearGameplayTags()
{
	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}
	
	AbilitySystemComponent->SetLooseGameplayTagCount(CollabGameplayTags::Status_Death_Dying, 0);
	AbilitySystemComponent->SetLooseGameplayTagCount(CollabGameplayTags::Status_Death_Dead, 0);
}

