// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CollabGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class COLLAB_API UCollabGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	virtual void Init() override;
};
