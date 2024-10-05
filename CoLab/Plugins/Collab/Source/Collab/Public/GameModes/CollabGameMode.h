// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CollabGameMode.generated.h"

class UCollabPawnData;
/**
 * 
 */
UCLASS()
class COLLAB_API ACollabGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	/** The default pawn class to spawn for players */
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TSoftObjectPtr<const UCollabPawnData> DefaultPawnData;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Collab|Pawn")
	const UCollabPawnData* GetPawnDataForController(const AController* InController) const;

	
	//~Begin AGameModeBase interface
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	//~End AGameModeBase interface
};
