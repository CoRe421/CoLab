// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CollabReplicatedComponentInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UCollabReplicatedComponentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COLLAB_API ICollabReplicatedComponentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetMaxDrawDistance(const float Distance);
	virtual void SetMaxDrawDistance_Implementation(const float Distance) {};
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DestroyComponent();
	virtual void DestroyComponent_Implementation();
};
