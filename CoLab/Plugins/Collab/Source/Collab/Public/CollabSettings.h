// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CollabSettings.generated.h"

/**
 * 
 */
UCLASS(DefaultConfig, Config=Game, DisplayName="Collab Settings")
class COLLAB_API UCollabSettings : public UDeveloperSettings
{
	GENERATED_BODY()

private:
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Collab|Startup")
	TSoftObjectPtr<class UWorld> MenuMap;
	
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Collab|Global")
	TSet<TSoftObjectPtr<class UCollabGameData>> RegisteredGameData;
};
