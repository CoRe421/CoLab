// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CoLabSurfRaceSettings.generated.h"

/**
 * 
 */
UCLASS(DefaultConfig, Config=Game, DisplayName="SurfRace Settings")
class COLAB_API UCoLabSurfRaceSettings final : public UDeveloperSettings
{
	GENERATED_BODY()

	UCoLabSurfRaceSettings();

private:
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Generation")
	float BlockWidth;

public:
	UFUNCTION(BlueprintCallable)
	float GetBlockWidth() { return BlockWidth; };
};
