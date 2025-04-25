// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Abilities/CollabGameplayAbility.h"

#include "CollabGameplayTags.h"

FGameplayTag UCollabGameplayAbility::GetInputTag() const
{
	return InputTag;
}

float UCollabGameplayAbility::GetCost(const int32 AbilityLevel /*= 1*/) const
{
	return Cost.GetValueAtLevel(AbilityLevel);
}

const FGameplayTagContainer* UCollabGameplayAbility::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); // MutableTags writes to the TempCooldownTags on the CDO so clear it in case the ability cooldown tags change (moved to a different slot)
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);
	return MutableTags;
}

void UCollabGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (!IsValid(CooldownGE))
	{
		return;
	}

	const auto AbilityLevel = GetAbilityLevel();
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), AbilityLevel);
	FGameplayEffectSpec* Data = SpecHandle.Data.Get();
	if (!ensureAlways(Data))
	{
		return;
	}
	Data->DynamicGrantedTags.AppendTags(CooldownTags);
	Data->SetSetByCallerMagnitude(CollabGameplayTags::Ability_Data_Cooldown, CooldownDuration.GetValueAtLevel(AbilityLevel));
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}
