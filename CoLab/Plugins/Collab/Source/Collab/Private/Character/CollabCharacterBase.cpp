// Fill out your copyright notice in the Description page of Project Settings.


#include "Character\CollabCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Character/CollabPawnExtensionComponent.h"
#include "GameplayEffectTypes.h"


// Sets default values
ACollabCharacterBase::ACollabCharacterBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACollabCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACollabCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACollabCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UCollabAbilitySystemComponent* ACollabCharacterBase::GetCollabAbilitySystemComponent() const
{
	return Cast<UCollabAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ACollabCharacterBase::GetAbilitySystemComponent() const
{
	if (!IsValid(PawnExtComponent))
	{
		return nullptr;
	}
	return PawnExtComponent->GetCollabAbilitySystemComponent();
}
