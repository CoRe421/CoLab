// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CollabGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class COLLAB_API UCollabGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	FScalableFloat Cost;

public:
	UFUNCTION(BlueprintPure, BlueprintCallable)
	FGameplayTag GetInputTag() const;
	
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetCost(const int32 AbilityLevel = 1) const;
};
