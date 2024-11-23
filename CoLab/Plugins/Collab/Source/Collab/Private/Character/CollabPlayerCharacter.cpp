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
	
	PawnExtensionComponent->HandleControllerChanged();
}

void ACollabPlayerCharacter::UnPossessed()
{
	Super::UnPossessed();
	
	PawnExtensionComponent->HandleControllerChanged();
}

void ACollabPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	PawnExtensionComponent->HandlePlayerStateReplicated();
}

void ACollabPlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
	
	PawnExtensionComponent->HandleControllerChanged();
}

void ACollabPlayerCharacter::OnAbilitySystemInitialized(UCollabAbilitySystemComponent* CollabASC)
{	
	OnAbilitySystemInit.Broadcast(CollabASC);
	
	// This needs to be called AFTER delegate so that everything will be initialized for this - CR
	SetupAbilitySystemComponentBindings();
}

void ACollabPlayerCharacter::SetupAbilitySystemComponentBindings_Implementation()
{
	UCollabAbilitySystemComponent* CollabASC = GetCollabAbilitySystemComponent();
	if (!IsValid(CollabASC))
	{
		return;
	}

	const TArray<UAttributeSet*>& AttributeSets = CollabASC->GetSpawnedAttributes();
	for (UAttributeSet* AttributeSet : AttributeSets)
	{
		UCollabAttributeSet* CollabAttribute = Cast<UCollabAttributeSet>(AttributeSet);
		if (!ensureAlways(IsValid(CollabAttribute)))
		{
			continue;
		}

		CollabAttribute->SetupBindings(CollabASC);
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

	PawnExtensionComponent->SetupPlayerInputComponent();
}