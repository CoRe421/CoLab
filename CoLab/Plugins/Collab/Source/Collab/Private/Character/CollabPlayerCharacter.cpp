// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CollabPlayerCharacter.h"

#include "Character/CollabPawnExtensionComponent.h"
#include "Player/CollabPlayerController.h"
#include "Player/CollabPlayerState.h"


// Sets default values
ACollabPlayerCharacter::ACollabPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACollabPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ACollabPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitAbilitySystemComponent();
}

void ACollabPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	InitAbilitySystemComponent();
}

// Called every frame
void ACollabPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACollabPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACollabPlayerCharacter::InitAbilitySystemComponent()
{
	ACollabPlayerState* CollabPlayerState = GetPlayerState<ACollabPlayerState>();
	if (!IsValid(CollabPlayerState))
	{
		return;
	} 
	
	if (UCollabPawnExtensionComponent* PawnExtComp = UCollabPawnExtensionComponent::FindPawnExtensionComponent(this))
	{
		// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
		// The ability system component and attribute sets live on the player state.
		PawnExtComp->InitializeAbilitySystem(CollabPlayerState->GetCollabAbilitySystemComponent(), CollabPlayerState);
	}
}