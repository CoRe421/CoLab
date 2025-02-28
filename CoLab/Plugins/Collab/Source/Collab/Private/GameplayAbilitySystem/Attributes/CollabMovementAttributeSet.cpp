// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Attributes/CollabMovementAttributeSet.h"

#include "Net/UnrealNetwork.h"

void UCollabMovementAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, JumpHeight, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabMovementAttributeSet, Mass, COND_None, REPNOTIFY_Always);
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
