// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/CollabActor.h"

#include "Components/CollabStaticMeshComponent.h"


// Sets default values
ACollabActor::ACollabActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bSmoothClientPhysics = false;

	NetRelevancyDistance = 5000.f;
}

void ACollabActor::SetNetRelevancyDistance(const float Distance)
{
	NetRelevancyDistance = Distance;
}

// Called when the game starts or when spawned
void ACollabActor::BeginPlay()
{
	Super::BeginPlay();
}

void ACollabActor::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
}

bool ACollabActor::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget,
	const FVector& SrcLocation) const
{
	if (!Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation))
	{
		return false;
	}

	const FVector CurrentLocation = GetActorLocation();
	const float DistanceFromActor = FVector::Distance(CurrentLocation, SrcLocation);

	return DistanceFromActor <= NetRelevancyDistance;
}

// Called every frame
void ACollabActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const ENetRole LocalRole = GetLocalRole();
	const bool bIsReplicatingMovement = IsReplicatingMovement();
	if (bSmoothClientPhysics && bIsReplicatingMovement && LocalRole == ENetRole::ROLE_SimulatedProxy)
	{
		TArray<UPrimitiveComponent*> Components;
		GetComponents<UPrimitiveComponent>(Components);

		for (UPrimitiveComponent* Component : Components)
		{
			if (!ensureAlways(IsValid(Component)))
			{
				continue;
			}

			// Don't know if this does anything, wouldn't be accessible if not replicated I think
			const bool bIsReplicated = Component->GetIsReplicated();
			if (!bIsReplicated)
			{
				continue;
			}

			const FRepMovement& LocalRepMovement = GetReplicatedMovement();
			if (!LocalRepMovement.bRepPhysics)
			{
				continue;
			}

			// LocalRepMovement.

			const FVector Velocity = LocalRepMovement.LinearVelocity;
			const FVector AngularVelocity = LocalRepMovement.AngularVelocity;

			if (Velocity.IsNearlyZero() && AngularVelocity.IsNearlyZero())
			{
				continue;
			}

			const FVector CurrentLocation = LocalRepMovement.Location;
			const FRotator CurrentRotation = Component->GetComponentRotation();
			const FVector CurrentScale = Component->GetComponentScale();

			const FVector NewLocation = CurrentLocation + (Velocity * DeltaTime);
			const FVector DeltaAngularVelocity = AngularVelocity * DeltaTime;
			const FRotator NewRotation = FRotator(CurrentRotation.Pitch + DeltaAngularVelocity.Y, CurrentRotation.Yaw + DeltaAngularVelocity.Z, CurrentRotation.Roll + DeltaAngularVelocity.X);

			FTransform NewTransform(NewRotation, NewLocation, CurrentScale);
			Component->SetWorldTransform(NewTransform);
		}
	}
}

