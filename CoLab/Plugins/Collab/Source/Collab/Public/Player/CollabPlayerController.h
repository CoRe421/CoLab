// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CollabPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class COLLAB_API ACollabPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void AcknowledgePossession(APawn* NewPawn) override;
};
