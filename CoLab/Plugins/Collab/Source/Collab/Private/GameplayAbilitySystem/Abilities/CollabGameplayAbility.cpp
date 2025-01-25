// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Abilities/CollabGameplayAbility.h"

FGameplayTag UCollabGameplayAbility::GetInputTag() const
{
	return InputTag;
}

float UCollabGameplayAbility::GetCost(const int32 AbilityLevel /*= 1*/) const
{
	return Cost.GetValueAtLevel(AbilityLevel);
}
