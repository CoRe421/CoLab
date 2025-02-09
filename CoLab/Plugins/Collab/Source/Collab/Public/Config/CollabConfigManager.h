// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollabConfigManager.generated.h"

class UCollabConfigData;

UCLASS(BlueprintType, Blueprintable)
class COLLAB_API ACollabConfigManager : public AActor
{
	GENERATED_BODY()

	friend class UCollabRuntimeConfigSubsystem;

private:
	UPROPERTY(Replicated)
	TArray<TObjectPtr<UCollabConfigData>> ConstructedConfigs;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Sets default values for this actor's properties
	ACollabConfigManager();

private:
	UCollabConfigData* FindOrCreateConfigData(const TSubclassOf<UCollabConfigData>& ConfigDataClass);

	virtual void BeginPlay() override;

	virtual bool IsSupportedForNetworking() const override;

private:
	// Used by UCollabRuntimeConfigSubsystem to preserve configs across levels - CR
	void InitializeConfigData(const TArray<TObjectPtr<UCollabConfigData>>& ConfigData);

	UCollabConfigData* ConstructNewConfigData(const TSubclassOf<UCollabConfigData>& ConfigDataClass, UCollabConfigData* ConfigDataTemplate = nullptr);
};
