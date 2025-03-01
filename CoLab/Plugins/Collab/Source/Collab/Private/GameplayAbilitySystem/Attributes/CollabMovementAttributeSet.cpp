// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Attributes/CollabMovementAttributeSet.h"

#include "Net/UnrealNetwork.h"

void UCollabMovementAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, JumpHeight, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, Mass, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, GravityScale, COND_None, REPNOTIFY_Always)
}

void UCollabMovementAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabMovementAttributeSet, MovementSpeed, OldMovementSpeed);
	COLLABATTRIBUTE_REPNOTIFY(GetMovementSpeedAttribute(), OldMovementSpeed.GetCurrentValue(), GetMovementSpeed());
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