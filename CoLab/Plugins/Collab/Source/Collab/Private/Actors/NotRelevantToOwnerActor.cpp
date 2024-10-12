// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/NotRelevantToOwnerActor.h"


// Sets default values
ANotRelevantToOwnerActor::ANotRelevantToOwnerActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANotRelevantToOwnerActor::BeginPlay()
{
	Super::BeginPlay();
	
}

bool ANotRelevantToOwnerActor::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget,
	const FVector& SrcLocation) const
{
	if (ViewTarget == GetOwner())
	{
		return false;
	}
	
	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}

// Called every frame
void ANotRelevantToOwnerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

