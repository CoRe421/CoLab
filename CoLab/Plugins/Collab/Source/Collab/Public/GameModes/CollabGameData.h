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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Collab|Default")
	FText DisplayName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Collab|Default")
	TSoftObjectPtr<class UWorld> GameLevel;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Collab|Settings")
	TMap<FGameplayTag, TSoftClassPtr<UCollabGameplayEffect>> GlobalTagEffects;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Collab|Config")
	TSet<TSoftClassPtr<class UCollabConfigData>> ConfigurableData;

public:	
	UFUNCTION(BlueprintCallable)
	const TMap<FGameplayTag, TSoftClassPtr<UCollabGameplayEffect>>& GetGlobalTagEffects() const;
};
