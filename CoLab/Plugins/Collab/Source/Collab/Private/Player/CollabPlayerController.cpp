// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CollabPlayerController.h"

#include "Character\CollabCharacterBase.h"

void ACollabPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	
	ACollabCharacterBase* CollabCharacter = Cast<ACollabCharacterBase>(NewPawn);
	if (IsValid(CollabCharacter))
	{
		CollabCharacter->GetAbilitySystemComponent()->InitAbilityActorInfo(CollabCharacter, CollabCharacter);
	}
}
