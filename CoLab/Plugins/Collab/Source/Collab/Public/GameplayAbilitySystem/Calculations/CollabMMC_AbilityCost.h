// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySystem/Abilities/CollabGameplayAbility.h"
#include "GameplayModMagnitudeCalculation.h"
#include "CollabMMC_AbilityCost.generated.h"

/**
 * 
 */
UCLASS()
class COLLAB_API UCollabMMC_AbilityCost : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UCollabMMC_AbilityCost();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
