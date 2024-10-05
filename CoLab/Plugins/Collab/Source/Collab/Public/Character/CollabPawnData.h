// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CollabPawnData.generated.h"


class UCollabInputConfig;
class UCollabAbilitySet;
class ACollabPlayerCharacter;
/**
 * UCollabPawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Collab Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class COLLAB_API UCollabPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UCollabPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// Class to instantiate for this pawn (should usually derive from ALyraPawn or ALyraCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collab|Pawn")
	TSubclassOf<ACollabPlayerCharacter> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collab|Abilities")
	TArray<TObjectPtr<UCollabAbilitySet>> AbilitySets;

	// // What mapping of ability tags to use for actions taking by this pawn
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collab|Abilities")
	// TObjectPtr<UCollabAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collab|Input")
	TObjectPtr<UCollabInputConfig> InputConfig;

	// // Default camera mode used by player controlled pawns.
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collab|Camera")
	// TSubclassOf<UCollabCameraMode> DefaultCameraMode;