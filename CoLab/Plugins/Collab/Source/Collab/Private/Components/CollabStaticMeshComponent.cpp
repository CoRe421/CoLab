// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CollabStaticMeshComponent.h"
#include "PhysicsEngine/PhysicsObjectExternalInterface.h"
#include "Runtime/Experimental/Chaos/Private/Chaos/PhysicsObjectInternal.h"
#include "Net/UnrealNetwork.h"

UCollabStaticMeshComponent::UCollabStaticMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	MaxDrawDistance = CachedMaxDrawDistance;
	bDirtyLinearVelocity = false;
	VelocityDiffRepCondition = 100.f;
}

void UCollabStaticMeshComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UCollabStaticMeshComponent, Rep_PhysicsLinearVelocity, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(UCollabStaticMeshComponent, Rep_PhysicsAngularVelocity, COND_SimulatedOnly);
	DOREPLIFETIME(UCollabStaticMeshComponent, MaxDrawDistance);
}

void UCollabStaticMeshComponent::OnRegister()
{
	Super::OnRegister();

	OnComponentConstruction();
}

void UCollabStaticMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Fires on server
void UCollabStaticMeshComponent::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
	
	const ENetRole LocalRole = GetOwnerRole();
	if (LocalRole != ROLE_Authority)
	{
		return;
	}
	
	// Don't know if this does anything, wouldn't be accessible if not replicated I think
	const bool bIsReplicated = GetIsReplicated();
	const bool bIsSimulatingPhysics = IsSimulatingPhysics();
	if (!bIsReplicated || !bIsSimulatingPhysics)
	{
		return;
	}

	const FVector LinearVelocity = GetPhysicsLinearVelocity();
	const FVector AngularVelocity = GetPhysicsAngularVelocityInDegrees();

	if (FVector::Dist(LinearVelocity, Rep_PhysicsLinearVelocity) > VelocityDiffRepCondition)
	{
		Rep_PhysicsLinearVelocity = LinearVelocity;
	}
	if (FVector::Dist(AngularVelocity, Rep_PhysicsAngularVelocity) > VelocityDiffRepCondition)
	{
		Rep_PhysicsAngularVelocity = AngularVelocity;
	}
	
	const bool bIsTicking = IsComponentTickEnabled();
	if (!bIsTicking && (!Rep_PhysicsLinearVelocity.IsNearlyZero() || !Rep_PhysicsAngularVelocity.IsNearlyZero()))
	{
		SetComponentTickEnabled(true);
	}
}

// Fires on client
void UCollabStaticMeshComponent::PostRepNotifies()
{
	Super::PostRepNotifies();
	
	const bool bIsTicking = IsComponentTickEnabled();
	if (!bIsTicking && !Rep_PhysicsLinearVelocity.IsNearlyZero())
	{
		SetComponentTickEnabled(true);
	}
}

void UCollabStaticMeshComponent::SetMaxDrawDistance_Implementation(const float Distance)
{
	ICollabReplicatedComponentInterface::SetMaxDrawDistance_Implementation(Distance);

	MaxDrawDistance = Distance;
	SetCullDistance(MaxDrawDistance);
}

void UCollabStaticMeshComponent::OnRep_PhysicsLinearVelocity(const FVector& OldLinearVelocity)
{
	bDirtyLinearVelocity = true;
}

void UCollabStaticMeshComponent::OnRep_MaxDrawDistance(const float& OldMaxDrawDistance)
{
	SetCullDistance(MaxDrawDistance);
}

void UCollabStaticMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Don't know if this does anything, wouldn't be accessible if not replicated I think
	const bool bIsReplicated = GetIsReplicated();
	const ENetRole OwnerRole = GetOwnerRole();
	int32 test = 1;
	if (bIsReplicated && OwnerRole == ROLE_SimulatedProxy)
	{
		if (Rep_PhysicsLinearVelocity.IsNearlyZero())
		{
			SetComponentTickEnabled(false);
		}
		else
		{
			const FVector CurrentLocation = GetComponentLocation();
			const FRotator CurrentRotation = GetComponentRotation();
			const FVector CurrentScale = GetComponentScale();

			FVector LinearVelocity = CachedLinearVelocity;
			if (bDirtyLinearVelocity)
			{
				LinearVelocity = Rep_PhysicsLinearVelocity;
				bDirtyLinearVelocity = false;
			}

			if (!FMath::IsNearlyZero(Rep_PhysicsLinearVelocity.Z))
			{
				UWorld* CurrentWorld = GetWorld();
				const float WorldGravity = CurrentWorld->GetGravityZ();
				const FVector GravityVelocity = WorldGravity * DeltaTime * FVector::ZAxisVector;
				LinearVelocity += GravityVelocity;
			}

			const FVector NewLocation = CurrentLocation + (LinearVelocity * DeltaTime);
			const FVector DeltaAngularVelocity = Rep_PhysicsAngularVelocity * DeltaTime;
			const FRotator NewRotation = FRotator(CurrentRotation.Pitch + DeltaAngularVelocity.Y, CurrentRotation.Yaw + DeltaAngularVelocity.Z, CurrentRotation.Roll + DeltaAngularVelocity.X);

			FTransform NewTransform(NewRotation, NewLocation, CurrentScale);
			SetWorldTransform(NewTransform);

			CachedLinearVelocity = LinearVelocity;
		}
	}
	else if (OwnerRole == ROLE_Authority)
	{
		const FVector LinearVelocity = GetPhysicsLinearVelocity();
		const FVector AngularVelocity = GetPhysicsAngularVelocityInDegrees();
		if (LinearVelocity.IsNearlyZero() && AngularVelocity.IsNearlyZero())
		{
			Rep_PhysicsLinearVelocity = FVector::ZeroVector;
			Rep_PhysicsAngularVelocity = FVector::ZeroVector;
			SetSimulatePhysics(false);
			// SetIsReplicated(false);
			SetComponentTickEnabled(false);
		}
	}

	OnComponentTick.Broadcast(this, DeltaTime);
}
