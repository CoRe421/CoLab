// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "CollabGameData.generated.h"

class UCollabGameplayEffect;
/**
 * 
 */
UCLASS(BlueprintType, Const)
class COLLAB_API UCollabGameData : public UPrimaryDataAsset
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess))
	TMap<FGameplayTag, TSoftClassPtr<UCollabGameplayEffect>> GlobalTagEffects;

public:	
	UFUNCTION(BlueprintCallable)
	const TMap<FGameplayTag, TSoftClassPtr<UCollabGameplayEffect>>& GetGlobalTagEffects() const;
};
