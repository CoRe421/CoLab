// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CollabPlayerCharacter.h"

#include "CollabLog.h"
#include "Character/CollabHeroComponent.h"
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

	HeroComponent = CreateDefaultSubobject<UCollabHeroComponent>(TEXT("HeroComponent"));
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

void ACollabPlayerCharacter::SetupAbilitySystemComponentBindings_Implementation()
{
	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}

	const TArray<UAttributeSet*>& AttributeSets = AbilitySystemComponent->GetSpawnedAttributes();
	for (UAttributeSet* AttributeSet : AttributeSets)
	{
		UCollabAttributeSet* CollabAttribute = Cast<UCollabAttributeSet>(AttributeSet);
		if (!ensureAlways(IsValid(CollabAttribute)))
		{
			continue;
		}

		CollabAttribute->SetupBindings(AbilitySystemComponent.Get());
	}
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
	
	// Placed here to be sure input component is valid - CR
	HeroComponent->InitializeHeroComponent();
}

void ACollabPlayerCharacter::InitAbilitySystemComponent()
{
	ACollabPlayerState* CollabPlayerState = GetPlayerState<ACollabPlayerState>();
	if (!IsValid(CollabPlayerState))
	{
		return;
	}

	//UCollabPawnExtensionComponent* PawnExtComp = UCollabPawnExtensionComponent::FindPawnExtensionComponent(this)
	AbilitySystemComponent = CollabPlayerState->GetCollabAbilitySystemComponent();
	if (AbilitySystemComponent.IsValid())
	{
		// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
		// The ability system component and attribute sets live on the player state.
		AbilitySystemComponent->InitAbilityActorInfo(CollabPlayerState, this);

		if (HasAuthority())
		{
			CollabPlayerState->ApplyDefaultGameplayEffects();
		}
		
		SetupAbilitySystemComponentBindings();

		// Broadcast ability system initialized here for death/health component to bind to
		OnAbilitySystemInit.Broadcast(AbilitySystemComponent.Get());
	}
}
