// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CollabEnemyCharacter.h"


// Sets default values
ACollabEnemyCharacter::ACollabEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
}

// Called when the game starts or when spawned
void ACollabEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACollabEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACollabEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

