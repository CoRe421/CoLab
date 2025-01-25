// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "CollabPlayerController.generated.h"

class UCollabAbilitySystemComponent;
struct FGameplayTag;
class UCollabInputConfig;
/**
 * 
 */
UCLASS()
class COLLAB_API ACollabPlayerController : public APlayerController, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UCollabAbilitySystemComponent* GetCollabAbilitySystemComponent() const;

protected:
	virtual void BeginPlay() override;
	
	virtual void AcknowledgePossession(APawn* NewPawn) override;
};
