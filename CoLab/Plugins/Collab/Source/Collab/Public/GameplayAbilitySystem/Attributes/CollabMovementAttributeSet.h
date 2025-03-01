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
	UPROPERTY(BlueprintReadOnly, Category = "Movement|Air", ReplicatedUsing = OnRep_AirControlBoostMultiplier, meta=(AllowPrivateAccess))
	FGameplayAttributeData AirControlBoostMultiplier;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, AirControlBoostMultiplier)
	UPROPERTY(BlueprintReadOnly, Category = "Movement|Air", ReplicatedUsing = OnRep_AirControlBoostVelocityThreshold, meta=(AllowPrivateAccess))
	FGameplayAttributeData AirControlBoostVelocityThreshold;
	ATTRIBUTE_ACCESSORS(UCollabMovementAttributeSet, AirControlBoostVelocityThreshold)

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed);
	UFUNCTION()
	virtual void OnRep_JumpHeight(const FGameplayAttributeData& OldJumpHeight);
	UFUNCTION()
	virtual void OnRep_Mass(const FGameplayAttributeData& OldMass);
	UFUNCTION()
	virtual void OnRep_GravityScale(const FGameplayAttributeData& OldGravityScale);
	UFUNCTION()
	virtual void OnRep_AirControl(const FGameplayAttributeData& OldAirControl);
	UFUNCTION()
	virtual void OnRep_AirControlBoostMultiplier(const FGameplayAttributeData& OldAirControlBoostMultiplier);
	UFUNCTION()
	virtual void OnRep_AirControlBoostVelocityThreshold(const FGameplayAttributeData& OldAirControlBoostVelocityThreshold);
};
