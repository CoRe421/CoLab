// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CollabPlayerController.h"

#include "Character\CollabCharacterBase.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"
#include "Input/CollabInputComponent.h"
#include "Player/CollabPlayerState.h"

UAbilitySystemComponent* ACollabPlayerController::GetAbilitySystemComponent() const
{
	return GetCollabAbilitySystemComponent();
}

UCollabAbilitySystemComponent* ACollabPlayerController::GetCollabAbilitySystemComponent() const
{
	ACollabPlayerState* CollabPlayerState = GetPlayerState<ACollabPlayerState>();
	if (IsValid(CollabPlayerState))
	{
		UCollabAbilitySystemComponent* AbilitySystemComponent = CollabPlayerState->GetCollabAbilitySystemComponent();
		return AbilitySystemComponent;
	}

	return nullptr;
}

void ACollabPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ACollabPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
}

void ACollabPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UCollabInputComponent* CollabInputComponent = Cast<UCollabInputComponent>(InputComponent);
	if (IsValid(CollabInputComponent))
	{
		UCollabInputConfig* InputConfig = CollabInputConfig.LoadSynchronous();
		if (IsValid(InputConfig))
		{
			CollabInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputPressed, &ThisClass::AbilityInputReleased);
		}
	}
}

void ACollabPlayerController::AbilityInputPressed(FGameplayTag InputTag)
{
	UCollabAbilitySystemComponent* CollabAbilitySystemComponent = GetCollabAbilitySystemComponent();
	if (IsValid(CollabAbilitySystemComponent))
	{
		CollabAbilitySystemComponent->AbilityInputPressed(InputTag);
	}
}

void ACollabPlayerController::AbilityInputReleased(FGameplayTag InputTag)
{
	UCollabAbilitySystemComponent* CollabAbilitySystemComponent = GetCollabAbilitySystemComponent();
	if (IsValid(CollabAbilitySystemComponent))
	{
		CollabAbilitySystemComponent->AbilityInputReleased(InputTag);
	}
}
