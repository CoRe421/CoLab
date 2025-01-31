// Fill out your copyright notice in the Description page of Project Settings.


#include "Config/TestReplication.h"


// Sets default values
ATestReplication::ATestReplication()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATestReplication::BeginPlay()
{
	Super::BeginPlay();

	SetReplicates(true);
}

// Called every frame
void ATestReplication::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

