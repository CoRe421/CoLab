// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CollabGameModeInterface.generated.h"

DECLARE_MULTICAST_DELEGATE(FNativeEvent_OnPlayerListUpdated);
DECLARE_DYNAMIC_DELEGATE(FDynamicCallback_OnPlayerListUpdated);

// This class does not need to be modified.
UINTERFACE(Blueprintable, BlueprintType)
class COLLAB_API UCollabGameModeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COLLAB_API ICollabGameModeInterface
{
	GENERATED_BODY()

protected:
	FNativeEvent_OnPlayerListUpdated OnPlayerListUpdated;

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SpawnPlayer(APlayerController* PlayerController);
	virtual void SpawnPlayer_Implementation(APlayerController* PlayerController) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BindOnPlayerListUpdated(const FDynamicCallback_OnPlayerListUpdated& Event);
	virtual void BindOnPlayerListUpdated_Implementation(const FDynamicCallback_OnPlayerListUpdated& Event) final;

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BroadcastPlayerListUpdated();
	virtual void BroadcastPlayerListUpdated_Implementation() final;
};
