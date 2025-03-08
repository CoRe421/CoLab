// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Attributes/CollabMovementAttributeSet.h"

#include "Net/UnrealNetwork.h"

UCollabMovementAttributeSet::UCollabMovementAttributeSet()
{
	MovementSpeed = 600.f;
	Acceleration = 2048.f;
	JumpHeight = 150.f;
	Mass = 300.f;
	GravityScale = 1.f;
	AirControl = .1f;
	AirControlBoostMultiplier = 0.f;
	AirControlBoostVelocityThreshold = 0.f;
	FallingLateralFriction = 2.f;
}

void UCollabMovementAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, Acceleration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, bAllowSlidingWhileMoving, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, JumpHeight, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, Mass, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, GravityScale, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, AirControl, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, AirSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, AirAcceleration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, AirControlBoostMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, AirControlBoostVelocityThreshold, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, FallingLateralFriction, COND_None, REPNOTIFY_Always);
}

void UCollabMovementAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabMovementAttributeSet, MovementSpeed, OldMovementSpeed);
	COLLABATTRIBUTE_REPNOTIFY(GetMovementSpeedAttribute(), OldMovementSpeed.GetCurrentValue(), GetMovementSpeed());
}

void UCollabMovementAttributeSet::OnRep_Acceleration(const FGameplayAttributeData& OldAcceleration)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabMovementAttributeSet, Acceleration, OldAcceleration);
	COLLABATTRIBUTE_REPNOTIFY(GetAccelerationAttribute(), OldAcceleration.GetCurrentValue(), GetAcceleration());
}

void UCollabMovementAttributeSet::OnRep_AllowSlidingWhileMoving(
	const FGameplayAttributeData& OldAllowSlidingWhileMoving)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabMovementAttributeSet, bAllowSlidingWhileMoving, OldAllowSlidingWhileMoving);
	COLLABATTRIBUTE_REPNOTIFY(GetbAllowSlidingWhileMovingAttribute(), OldAllowSlidingWhileMoving.GetCurrentValue(), GetbAllowSlidingWhileMoving());
}

void UCollabMovementAttributeSet::OnRep_JumpHeight(const FGameplayAttributeData& OldJumpHeight)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabMovementAttributeSet, JumpHeight, OldJumpHeight);
	COLLABATTRIBUTE_REPNOTIFY(GetJumpHeightAttribute(), OldJumpHeight.GetCurrentValue(), GetJumpHeight());
}

void UCollabMovementAttributeSet::OnRep_Mass(const FGameplayAttributeData& OldMass)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabMovementAttributeSet, Mass, OldMass);
	COLLABATTRIBUTE_REPNOTIFY(GetMassAttribute(), OldMass.GetCurrentValue(), GetMass());
}

void UCollabMovementAttributeSet::OnRep_GravityScale(const FGameplayAttributeData& OldGravityScale)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabMovementAttributeSet, GravityScale, OldGravityScale);
	COLLABATTRIBUTE_REPNOTIFY(GetGravityScaleAttribute(), OldGravityScale.GetCurrentValue(), GetGravityScale());
}

void UCollabMovementAttributeSet::OnRep_AirControl(const FGameplayAttributeData& OldAirControl)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabMovementAttributeSet, AirControl, OldAirControl);
	COLLABATTRIBUTE_REPNOTIFY(GetAirControlAttribute(), OldAirControl.GetCurrentValue(), GetAirControl());
}

void UCollabMovementAttributeSet::OnRep_AirSpeed(const FGameplayAttributeData& OldAirSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabMovementAttributeSet, AirSpeed, OldAirSpeed);
	COLLABATTRIBUTE_REPNOTIFY(GetAirSpeedAttribute(), OldAirSpeed.GetCurrentValue(), GetAirSpeed());
}

void UCollabMovementAttributeSet::OnRep_AirAcceleration(const FGameplayAttributeData& OldAirAcceleration)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabMovementAttributeSet, AirAcceleration, OldAirAcceleration);
	COLLABATTRIBUTE_REPNOTIFY(GetAirAccelerationAttribute(), OldAirAcceleration.GetCurrentValue(), GetAirAcceleration());
}

void UCollabMovementAttributeSet::OnRep_AirControlBoostMultiplier(
	const FGameplayAttributeData& OldAirControlBoostMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabMovementAttributeSet, AirControlBoostMultiplier, OldAirControlBoostMultiplier);
	COLLABATTRIBUTE_REPNOTIFY(GetAirControlBoostMultiplierAttribute(), OldAirControlBoostMultiplier.GetCurrentValue(), GetAirControlBoostMultiplier());
}

void UCollabMovementAttributeSet::OnRep_AirControlBoostVelocityThreshold(
	const FGameplayAttributeData& OldAirControlBoostVelocityThreshold)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabMovementAttributeSet, AirControlBoostVelocityThreshold, OldAirControlBoostVelocityThreshold);
	COLLABATTRIBUTE_REPNOTIFY(GetAirControlBoostVelocityThresholdAttribute(), OldAirControlBoostVelocityThreshold.GetCurrentValue(), GetAirControlBoostVelocityThreshold());
}

void UCollabMovementAttributeSet::OnRep_FallingLateralFriction(const FGameplayAttributeData& OldFallingLateralFriction)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabMovementAttributeSet, FallingLateralFriction, OldFallingLateralFriction);
	COLLABATTRIBUTE_REPNOTIFY(GetFallingLateralFrictionAttribute(), OldFallingLateralFriction.GetCurrentValue(), GetFallingLateralFriction());
}


void UCollabMovementAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue,
	float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetbAllowSlidingWhileMovingAttribute())
	{
		bAllowSlidingWhileMoving = FMath::RoundToFloat(FMath::Clamp(NewValue, 0.f, 1.f));
	}
}