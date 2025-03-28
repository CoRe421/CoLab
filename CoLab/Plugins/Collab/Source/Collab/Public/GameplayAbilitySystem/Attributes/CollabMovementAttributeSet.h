// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollabAttributeSet.h"
#include "CollabMovementAttributeSet.generated.h"

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynamicEvent_OnMovementSpeedChanged, const float, OldMovementSpeed, const float, NewMovementSpeed);
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynamicEvent_OnJumpHeightChanged, const float, OldJumpHeight, const float, NewJumpHeight);

/**
 * 
 */
UCLASS(BlueprintType)
class COLLAB_API UCollabMovementAttributeSet : public UCollabAttributeSet
{
	GENERATED_BODY()

public:
	// UPROPERTY(BlueprintAssignable, BlueprintReadOnly, DisplayName="OnMovementSpeedChanged")
	// mutable FDynamicEvent_OnMovementSpeedChanged BP_OnMovementSpeedChanged;
	// mutable FNativeEvent_CollabAttributeEvent OnMovementSpeedChanged;
	// UPROPERTY(BlueprintAssignable, BlueprintReadOnly, DisplayName="OnJumpHeightChanged")
	// mutable FDynamicEvent_OnJumpHeightChanged BP_OnJumpHeightChanged;
	// mutable FNativeEvent_CollabAttributeEvent OnJumpHeightChanged;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_MovementSpeed, meta=(AllowPrivateAccess))
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, MovementSpeed)
	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_Acceleration, meta=(AllowPrivateAccess))
	FGameplayAttributeData Acceleration;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, Acceleration)
	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_GroundFriction, meta=(AllowPrivateAccess))
	FGameplayAttributeData GroundFriction;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, GroundFriction)
	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_AllowSlidingWhileMoving, meta=(AllowPrivateAccess))
	FGameplayAttributeData bAllowSlidingWhileMoving;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, bAllowSlidingWhileMoving)
	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_LandingAccelerationGracePeriod, meta=(AllowPrivateAccess))
	FGameplayAttributeData LandingAccelerationGracePeriod;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, LandingAccelerationGracePeriod)
	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_LandingFrictionGracePeriod, meta=(AllowPrivateAccess))
	FGameplayAttributeData LandingFrictionGracePeriod;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, LandingFrictionGracePeriod)
	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_JumpHeight, meta=(AllowPrivateAccess))
	FGameplayAttributeData JumpHeight;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, JumpHeight)
	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_Mass, meta=(AllowPrivateAccess))
	FGameplayAttributeData Mass;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, Mass)
	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_GravityScale, meta=(AllowPrivateAccess))
	FGameplayAttributeData GravityScale;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, GravityScale)
	UPROPERTY(BlueprintReadOnly, Category = "Movement|Air", ReplicatedUsing = OnRep_AirControl, meta=(AllowPrivateAccess))
	FGameplayAttributeData AirControl;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, AirControl)
	UPROPERTY(BlueprintReadOnly, Category = "Movement|Air", ReplicatedUsing = OnRep_AirSpeed, meta=(AllowPrivateAccess))
	FGameplayAttributeData AirSpeed;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, AirSpeed)
	UPROPERTY(BlueprintReadOnly, Category = "Movement|Air", ReplicatedUsing = OnRep_AirAcceleration, meta=(AllowPrivateAccess))
	FGameplayAttributeData AirAcceleration;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, AirAcceleration)
	UPROPERTY(BlueprintReadOnly, Category = "Movement|Air", ReplicatedUsing = OnRep_AirControlBoostMultiplier, meta=(AllowPrivateAccess))
	FGameplayAttributeData AirControlBoostMultiplier;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, AirControlBoostMultiplier)
	UPROPERTY(BlueprintReadOnly, Category = "Movement|Air", ReplicatedUsing = OnRep_AirControlBoostVelocityThreshold, meta=(AllowPrivateAccess))
	FGameplayAttributeData AirControlBoostVelocityThreshold;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, AirControlBoostVelocityThreshold)
	UPROPERTY(BlueprintReadOnly, Category = "Movement|Air", ReplicatedUsing = OnRep_FallingLateralFriction, meta=(AllowPrivateAccess))
	FGameplayAttributeData FallingLateralFriction;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, FallingLateralFriction)
	UPROPERTY(BlueprintReadOnly, Category = "Movement|Air", ReplicatedUsing = OnRep_MaxGravityVelocity, meta=(AllowPrivateAccess))
	FGameplayAttributeData MaxGravityVelocity;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, MaxGravityVelocity)

	UCollabMovementAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed);
	UFUNCTION()
	virtual void OnRep_Acceleration(const FGameplayAttributeData& OldAcceleration);
	UFUNCTION()
	virtual void OnRep_GroundFriction(const FGameplayAttributeData& OldGroundFriction);
	UFUNCTION()
	virtual void OnRep_AllowSlidingWhileMoving(const FGameplayAttributeData& OldAllowSlidingWhileMoving);
	UFUNCTION()
	virtual void OnRep_LandingAccelerationGracePeriod(const FGameplayAttributeData& OldLandingAccelerationGracePeriod);
	UFUNCTION()
	virtual void OnRep_LandingFrictionGracePeriod(const FGameplayAttributeData& OldLandingFrictionGracePeriod);
	UFUNCTION()
	virtual void OnRep_JumpHeight(const FGameplayAttributeData& OldJumpHeight);
	UFUNCTION()
	virtual void OnRep_Mass(const FGameplayAttributeData& OldMass);
	UFUNCTION()
	virtual void OnRep_GravityScale(const FGameplayAttributeData& OldGravityScale);
	UFUNCTION()
	virtual void OnRep_AirControl(const FGameplayAttributeData& OldAirControl);
	UFUNCTION()
	virtual void OnRep_AirSpeed(const FGameplayAttributeData& OldAirSpeed);
	UFUNCTION()
	virtual void OnRep_AirAcceleration(const FGameplayAttributeData& OldAirAcceleration);
	UFUNCTION()
	virtual void OnRep_AirControlBoostMultiplier(const FGameplayAttributeData& OldAirControlBoostMultiplier);
	UFUNCTION()
	virtual void OnRep_AirControlBoostVelocityThreshold(const FGameplayAttributeData& OldAirControlBoostVelocityThreshold);
	UFUNCTION()
	virtual void OnRep_FallingLateralFriction(const FGameplayAttributeData& OldFallingLateralFriction);
	UFUNCTION()
	virtual void OnRep_MaxGravityVelocity(const FGameplayAttributeData& OldMaxGravityVelocity);

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
};