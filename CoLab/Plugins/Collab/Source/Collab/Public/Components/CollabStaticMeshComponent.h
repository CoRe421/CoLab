// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollabReplicatedComponentInterface.h"
#include "Components/StaticMeshComponent.h"
#include "CollabStaticMeshComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynamicEvent_OnComponentTick, UCollabStaticMeshComponent*, Component, float, DeltaTime);

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COLLAB_API UCollabStaticMeshComponent : public UStaticMeshComponent, public ICollabReplicatedComponentInterface
{
	GENERATED_BODY()

protected:
	TOptional<FVector> CachedLocation;
	TOptional<FQuat> CachedRotation;
	TOptional<FVector> CachedVelocity;
	TOptional<FVector> CachedRotVelocity;

	bool bUpdatingNetData = false;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_PhysicsLinearVelocity)
	FVector Rep_PhysicsLinearVelocity;
	bool bDirtyLinearVelocity;
	FVector CachedLinearVelocity;
	UPROPERTY(Replicated)
	FVector Rep_PhysicsAngularVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	float VelocityDiffRepCondition;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_MaxDrawDistance)
	float MaxDrawDistance;

public:
	UPROPERTY(BlueprintReadWrite, BlueprintAssignable)
	FDynamicEvent_OnComponentTick OnComponentTick;

protected:
	UCollabStaticMeshComponent();

	// virtual void AsyncPhysicsTickComponent(float DeltaTime, float SimTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

	virtual void PostRepNotifies() override;

	virtual void SetMaxDrawDistance_Implementation(const float Distance) override;
	
	UFUNCTION()
	virtual void OnRep_PhysicsLinearVelocity(const FVector& OldLinearVelocity);
	UFUNCTION()
	virtual void OnRep_MaxDrawDistance(const float& OldMaxDrawDistance);
};
