// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CollabPlayerCharacter.h"

#include "CollabLog.h"
#include "Character/CollabPawnExtensionComponent.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"
#include "Kismet/KismetStringLibrary.h"
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

	if (HasAuthority())
	{
		InitAbilitySystemComponent();
	}
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

void ACollabPlayerCharacter::LogNames()
{
	ACollabPlayerState* CollabPlayerState = GetPlayerState<ACollabPlayerState>();
	if (!IsValid(CollabPlayerState))
	{
		return;
	}
	AActor* PlayerStatePawn = CollabPlayerState->GetPawn();
	UE_LOG(LogCollab, Log, TEXT("Player State Pawn: %s"), *PlayerStatePawn->GetName());
	
	UE_LOG(LogCollab, Warning, TEXT("Char '%s', PS '%s', PC '%s', ASC '%s', Auth '%s'"), *GetName(), *CollabPlayerState->GetName(), GetLocalViewingPlayerController() ? *GetLocalViewingPlayerController()->GetName() : TEXT("Null"), *AbilitySystemComponent->GetOwner()->GetName(), *UKismetStringLibrary::Conv_BoolToString(HasAuthority()));
	SVR_LogNames();
}

void ACollabPlayerCharacter::SVR_LogNames_Implementation()
{
	ACollabPlayerState* CollabPlayerState = GetPlayerState<ACollabPlayerState>();
	if (!IsValid(CollabPlayerState))
	{
		return;
	}
	AActor* PlayerStatePawn = CollabPlayerState->GetPawn();
	UE_LOG(LogCollab, Log, TEXT("Player State Pawn: %s"), *PlayerStatePawn->GetName());
	
	UE_LOG(LogCollab, Error, TEXT("Char '%s', PS '%s', PC '%s', ASC '%s', Auth '%s'"), *GetName(), *CollabPlayerState->GetName(), GetLocalViewingPlayerController() ? *GetLocalViewingPlayerController()->GetName() : TEXT("Null"), *AbilitySystemComponent->GetOwner()->GetName(), *UKismetStringLibrary::Conv_BoolToString(HasAuthority()));
}

void ACollabPlayerCharacter::InitAbilitySystemComponent()
{
	ACollabPlayerState* CollabPlayerState = GetPlayerState<ACollabPlayerState>();
	if (!IsValid(CollabPlayerState))
	{
		return;
	}
	if (AbilitySystemComponent.IsValid())
	{
		return;
	}
	// if (GetLocalRole() == ROLE_SimulatedProxy)
	// {
	// 	return;
	// }
	// AActor* PlayerStatePawn = CollabPlayerState->GetPawn();
	// UE_LOG(LogCollab, Log, TEXT("Player State Pawn: %s"), *PlayerStatePawn->GetName());

	//UCollabPawnExtensionComponent* PawnExtComp = UCollabPawnExtensionComponent::FindPawnExtensionComponent(this)
	AbilitySystemComponent = CollabPlayerState->GetCollabAbilitySystemComponent();
	// if (!HasAuthority())
	// {
	// 	UE_LOG(LogCollab, Log, TEXT("Initializing client ASC"));
	// }
	if (AbilitySystemComponent.IsValid())
	{
		UE_LOG(LogCollab, Log, TEXT("Remote: %s"), *UEnum::GetValueAsString(GetRemoteRole()));
		UE_LOG(LogCollab, Log, TEXT("Local: %s"), *UEnum::GetValueAsString(GetLocalRole()));
		LogNames();
		// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
		// The ability system component and attribute sets live on the player state.
		AbilitySystemComponent->InitAbilityActorInfo(CollabPlayerState, this);
	}
}
