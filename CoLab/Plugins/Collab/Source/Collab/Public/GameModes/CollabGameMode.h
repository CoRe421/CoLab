// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameModeBase.h"
#include "CollabGameMode.generated.h"

class UCollabGameData;
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
	
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TSoftObjectPtr<const UCollabGameData> DefaultGameData;
	
public:
	static const UCollabGameData* GetDefaultGameData(const AActor* WorldContext);
	
	UFUNCTION(BlueprintCallable, Category = "Collab|Pawn")
	const UCollabPawnData* GetPawnDataForController(const AController* InController) const;

protected:
	//~Begin AGameModeBase interface
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void InitGameState() override;
	virtual void AddInactivePlayer(APlayerState* PlayerState, APlayerController* PC) override;
	//~End AGameModeBase interface

	//~Begin AGameMode interface
	virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;
	virtual void HandleMatchHasStarted() override;
	//~End AGameMode interface

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void StartPlay() override;

	UFUNCTION(BlueprintCallable)
	bool TryRestartPlayer(APlayerController* Player);

private:
	void RestartPlayers();
};
