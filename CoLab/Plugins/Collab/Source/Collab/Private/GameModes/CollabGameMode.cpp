// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/CollabGameMode.h"

#include "CollabLog.h"
#include "Character/CollabPawnData.h"
#include "Character/CollabPawnExtensionComponent.h"
#include "Character/CollabPlayerCharacter.h"
#include "Player/CollabPlayerState.h"

const UCollabPawnData* ACollabGameMode::GetPawnDataForController(const AController* InController) const
{
	// See if pawn data is already set on the player state
	if (IsValid(InController))
	{
		if (const ACollabPlayerState* CollabPS = InController->GetPlayerState<ACollabPlayerState>())
		{
			if (const UCollabPawnData* PawnData = CollabPS->GetPawnData<UCollabPawnData>())
			{
				return PawnData;
			}
		}
	}

	const UCollabPawnData* LoadedPawnData = Cast<UCollabPawnData>(DefaultPawnData.LoadSynchronous());
	return LoadedPawnData;
}

UClass* ACollabGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UCollabPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}
	
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* ACollabGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,
	const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// Never save the default player pawns into a map.
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (UCollabPawnExtensionComponent* PawnExtComp = UCollabPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UCollabPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
				else
				{
					UE_LOG(LogCollab, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."), *GetNameSafe(SpawnedPawn));
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);

			return SpawnedPawn;
		}
		else
		{
			UE_LOG(LogCollab, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		UE_LOG(LogCollab, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}

	return nullptr;
}
