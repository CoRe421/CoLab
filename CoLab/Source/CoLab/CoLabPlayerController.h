// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Player/CollabPlayerController.h"
#include "CoLabPlayerController.generated.h"

/**
 *
 */
UCLASS()
class COLAB_API ACoLabPlayerController : public ACollabPlayerController
{
	GENERATED_BODY()

	// Begin Actor interface
protected:

	virtual void BeginPlay() override;

	// End Actor interface
};
