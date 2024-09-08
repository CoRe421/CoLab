// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoLabGameMode.h"
#include "CoLabCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACoLabGameMode::ACoLabGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
