// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Abilities/CollabGameplayAbility_Death.h"

#include "CollabGameplayTags.h"
#include "Character/CollabCharacterBase.h"
#include "Character/CollabDeathComponent.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"

UCollabGameplayAbility_Death::UCollabGameplayAbility_Death(const FObjectInitializer& ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	bAutoStartDeath = true;

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		// Add the ability trigger tag as default to the CDO.
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = CollabGameplayTags::GameplayEvent_Death;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UCollabGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	check(ActorInfo);

	UCollabAbilitySystemComponent* CollabASC = CastChecked<UCollabAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

	FGameplayTagContainer AbilityTypesToIgnore;
	AbilityTypesToIgnore.AddTag(CollabGameplayTags::Ability_Behavior_SurvivesDeath);

	// Cancel all abilities and block others from starting.
	CollabASC->CancelAbilities(nullptr, &AbilityTypesToIgnore, this);

	SetCanBeCanceled(false);

	// if (!ChangeActivationGroup(ELyraAbilityActivationGroup::Exclusive_Blocking))
	// {
	// 	UE_LOG(LogLyraAbilitySystem, Error, TEXT("ULyraGameplayAbility_Death::ActivateAbility: Ability [%s] failed to change activation group to blocking."), *GetName());
	// }

	if (bAutoStartDeath)
	{
		StartDeath();
	}
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UCollabGameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	check(ActorInfo);

	// Always try to finish the death when the ability ends in case the ability doesn't.
	// This won't do anything if the death hasn't been started.
	FinishDeath();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCollabGameplayAbility_Death::StartDeath()
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	UCollabDeathComponent* DeathComponent = UCollabDeathComponent::FindDeathComponent(AvatarActor);
	if (!IsValid(DeathComponent))
	{
		return;
	}

	if (DeathComponent->GetDeathState() == ECollabDeathState::NotDead)
	{
		DeathComponent->StartDeath();
	}
}

void UCollabGameplayAbility_Death::FinishDeath()
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	UCollabDeathComponent* DeathComponent = UCollabDeathComponent::FindDeathComponent(AvatarActor);
	if (!IsValid(DeathComponent))
	{
		return;
	}

	if (DeathComponent->GetDeathState() == ECollabDeathState::DeathStarted)
	{
		DeathComponent->FinishDeath();
	}
}
