// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Attributes/CollabSpellcastAttributeSet.h"

#include "Net/UnrealNetwork.h"


void UCollabSpellcastAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabSpellcastAttributeSet, MaxMana, OldMaxMana);
}

void UCollabSpellcastAttributeSet::OnRep_CurrentMana(const FGameplayAttributeData& OldCurrentMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabSpellcastAttributeSet, CurrentMana, OldCurrentMana);
}

void UCollabSpellcastAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabSpellcastAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabSpellcastAttributeSet, CurrentMana, COND_None, REPNOTIFY_Always);
}
