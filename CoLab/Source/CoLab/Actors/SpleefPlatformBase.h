// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpleefPlatformBase.generated.h"

UCLASS()
class COLAB_API ASpleefPlatformBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpleefPlatformBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
