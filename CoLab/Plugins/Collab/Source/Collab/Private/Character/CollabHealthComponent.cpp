// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CollabHealthComponent.h"

#include "CollabGameplayTags.h"
#include "CollabLog.h"
#include "Character/CollabCharacterBase.h"
#include "GameModes/CollabGameData.h"
#include "GameModes/CollabGameMode.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"
#include "GameplayAbilitySystem/Attributes/CollabHealthAttributeSet.h"
#include "GameplayAbilitySystem/Effects/CollabGameplayEffect.h"
#include "Net/UnrealNetwork.h"


UCollabHealthComponent::UCollabHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
}

void UCollabHealthComponent::InitializeWithAbilitySystem(UCollabAbilitySystemComponent* InASC)
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

	HealthSet = AbilitySystemComponent->GetSet<UCollabHealthAttributeSet>();
	if (!HealthSet.IsValid())
	{
		UE_LOG(LogCollab, Error, TEXT("CollabHealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."), *GetNameSafe(Owner));
		return;
	}

	// Register to listen for attribute changes.
	HealthSet->OnHealthChanged.AddUObject(this, &ThisClass::HandleHealthChanged);
	// HealthSet->OnMaxHealthChanged.AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);

	// OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	// OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
}

void UCollabHealthComponent::UninitializeFromAbilitySystem()
{
	if (HealthSet.IsValid())
	{
		HealthSet->OnHealthChanged.RemoveAll(this);
		// HealthSet->OnMaxHealthChanged.RemoveAll(this);
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	HealthSet = nullptr;
	AbilitySystemComponent = nullptr;
}

// float UCollabHealthComponent::GetHealth() const
// {
// 	return (HealthSet ? HealthSet->GetHealth() : 0.0f);
// }
//
// float UCollabHealthComponent::GetMaxHealth() const
// {
// 	return (HealthSet ? HealthSet->GetMaxHealth() : 0.0f);
// }

// float UCollabHealthComponent::GetHealthNormalized() const
// {
// 	if (HealthSet)
// 	{
// 		const float Health = HealthSet->GetHealth();
// 		const float MaxHealth = HealthSet->GetMaxHealth();
//
// 		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
// 	}
//
// 	return 0.0f;
// }
//
// void UCollabHealthComponent::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
// {
// 	OnHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
// }
//
// void UCollabHealthComponent::HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
// {
// 	OnMaxHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
// }


void UCollabHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)
{
	// if ((DeathState == ECollabDeathState::NotDead) && AbilitySystemComponent)
	// {
	// 	const TSubclassOf<UGameplayEffect> DamageGE = UCollabAssetManager::GetSubclass(UCollabGameData::Get().DamageGameplayEffect_SetByCaller);
	// 	if (!DamageGE)
	// 	{
	// 		UE_LOG(LogCollab, Error, TEXT("CollabHealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to find gameplay effect [%s]."), *GetNameSafe(GetOwner()), *UCollabGameData::Get().DamageGameplayEffect_SetByCaller.GetAssetName());
	// 		return;
	// 	}
	//
	// 	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DamageGE, 1.0f, AbilitySystemComponent->MakeEffectContext());
	// 	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	//
	// 	if (!Spec)
	// 	{
	// 		UE_LOG(LogCollab, Error, TEXT("CollabHealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to make outgoing spec for [%s]."), *GetNameSafe(GetOwner()), *GetNameSafe(DamageGE));
	// 		return;
	// 	}
	//
	// 	Spec->AddDynamicAssetTag(TAG_Gameplay_DamageSelfDestruct);
	//
	// 	if (bFellOutOfWorld)
	// 	{
	// 		Spec->AddDynamicAssetTag(TAG_Gameplay_FellOutOfWorld);
	// 	}
	//
	// 	const float DamageAmount = GetMaxHealth();
	//
	// 	Spec->SetSetByCallerMagnitude(CollabGameplayTags::SetByCaller_Damage, DamageAmount);
	// 	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
	// }
}

void UCollabHealthComponent::OnRegister()
{
	Super::OnRegister();
	
	AActor* Owner = GetOwner();
	check(Owner);

	ACollabCharacterBase* CollabCharacterOwner = Cast<ACollabCharacterBase>(Owner);
	if (IsValid(CollabCharacterOwner))
	{
		CollabCharacterOwner->OnAbilitySystemInit.AddUniqueDynamic(this, &ThisClass::InitializeWithAbilitySystem);
	}
}

void UCollabHealthComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();

	Super::OnUnregister();
}

void UCollabHealthComponent::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	BroadcastDamaged();
}


void UCollabHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
#if WITH_SERVER_CODE
	if (AbilitySystemComponent.IsValid() && DamageEffectSpec)
	{
		// Send the "GameplayEvent.Death" gameplay event through the owner's ability system.  This can be used to trigger a death gameplay ability.
		{
			FGameplayEventData Payload;
			Payload.EventTag = CollabGameplayTags::GameplayEvent_Death;
			Payload.Instigator = DamageInstigator;
			Payload.Target = AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec->Def;
			Payload.ContextHandle = DamageEffectSpec->GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;

			FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent.Get(), true);
			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}

		// // Send a standardized verb message that other systems can observe
		// {
		// 	FCollabVerbMessage Message;
		// 	Message.Verb = TAG_Collab_Elimination_Message;
		// 	Message.Instigator = DamageInstigator;
		// 	Message.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
		// 	Message.Target = UCollabVerbMessageHelpers::GetPlayerStateFromObject(AbilitySystemComponent->GetAvatarActor());
		// 	Message.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
		// 	//@TODO: Fill out context tags, and any non-ability-system source/instigator tags
		// 	//@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...
		//
		// 	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
		// 	MessageSystem.BroadcastMessage(Message.Verb, Message);
		// }

		//@TODO: assist messages (could compute from damage dealt elsewhere)?
	}

#endif // #if WITH_SERVER_CODE
}

void UCollabHealthComponent::BroadcastDamaged()
{
	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}

	const AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	const UCollabGameData* GameData = ACollabGameMode::GetDefaultGameData(Owner);
	if (!IsValid(GameData))
	{
		return;
	}

	const TMap<FGameplayTag, TSoftClassPtr<UCollabGameplayEffect>>& GlobalTagEffects = GameData->GetGlobalTagEffects();
	const TSoftClassPtr<UCollabGameplayEffect>* DamagedEffectClass = GlobalTagEffects.Find(
		CollabHealthGameplayTags::Gameplay_State_Damaged);
	if (!DamagedEffectClass)
	{
		return;
	}

	const TSubclassOf<UCollabGameplayEffect> LoadedDamagedEffectClass = DamagedEffectClass->LoadSynchronous();
	if (!IsValid(LoadedDamagedEffectClass))
	{
		return;
	}

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(LoadedDamagedEffectClass, 1.0f, AbilitySystemComponent->MakeEffectContext());
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

	if (!Spec)
	{
		UE_LOG(LogCollab, Error, TEXT("CollabHealthComponent: BroadcastDamaged failed for owner [%s]. Unable to make outgoing spec for [%s]."), *GetNameSafe(GetOwner()), *GetNameSafe(LoadedDamagedEffectClass));
		return;
	}

	// Spec->AddDynamicAssetTag(TAG_Gameplay_DamageSelfDestruct);
	//
	// if (bFellOutOfWorld)
	// {
	// 	Spec->AddDynamicAssetTag(TAG_Gameplay_FellOutOfWorld);
	// }
	//
	// const float DamageAmount = GetMaxHealth();
	//
	// Spec->SetSetByCallerMagnitude(LyraGameplayTags::SetByCaller_Damage, DamageAmount);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
}
