// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Attributes/CollabSpellcastAttributeSet.h"

#include "Net/UnrealNetwork.h"


void UCollabSpellcastAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabSpellcastAttributeSet, MaxMana, OldMaxMana);
}

void UCollabSpellcastAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabSpellcastAttributeSet, Mana, OldMana);

	const float CurrentMana = GetMana();
	OnManaChanged.Broadcast(CurrentMana, MaxMana.GetCurrentValue());
}

void UCollabSpellcastAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabSpellcastAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabSpellcastAttributeSet, Mana, COND_None, REPNOTIFY_Always);
}

bool UCollabSpellcastAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	
	// Save the current health
	ManaBeforeAttributeChange = GetMana();
	
	return true;
}

void UCollabSpellcastAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// Use this function to trigger in-game reactions to attribute changes
	const float CurrentMana = GetMana();
	if (CurrentMana != ManaBeforeAttributeChange)
	{
		OnManaChanged.Broadcast(CurrentMana, GetMaxMana());
	}
}
