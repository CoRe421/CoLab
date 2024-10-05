// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CollabAbilityCharacter.h"


// Sets default values
ACollabAbilityCharacter::ACollabAbilityCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CharacterASC = CreateDefaultSubobject<UCollabAbilitySystemComponent>(TEXT("CollabCharacterASC"));
	CharacterASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	CharacterAttributeSet = CreateDefaultSubobject<UCollabAttributeSetBase>("AttributeSet");
}

// Called when the game starts or when spawned
void ACollabAbilityCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(CharacterASC))
	{
		CharacterASC->InitAbilityActorInfo(this, this);
	}
}

// Called every frame
void ACollabAbilityCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACollabAbilityCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

