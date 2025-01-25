// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Calculations/CollabMMC_AbilityCost.h"

UCollabMMC_AbilityCost::UCollabMMC_AbilityCost()
{
}

float UCollabMMC_AbilityCost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayEffectContextHandle Context = Spec.GetContext();
	const UGameplayAbility* Ability = Context.GetAbilityInstance_NotReplicated();
	const UCollabGameplayAbility* CollabAbility = Cast<UCollabGameplayAbility>(Ability);

	if (!IsValid(CollabAbility))
	{
		return 0.0f;
	}

	const int32 AbilityLevel = Ability->GetAbilityLevel();
	const float AbilityCost = CollabAbility->GetCost(AbilityLevel);
	return AbilityCost;
}
