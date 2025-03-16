// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Attributes/CollabSpellcastAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"


namespace CollabSpellcastGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_State_Spellcast, "Gameplay.State.Spellcast", "Target has recently cast a spell.");
}

void UCollabSpellcastAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabSpellcastAttributeSet, MaxMana, OldMaxMana);
	COLLABATTRIBUTE_REPNOTIFY(GetMaxManaAttribute(), OldMaxMana.GetCurrentValue(), GetMaxMana());
}

void UCollabSpellcastAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabSpellcastAttributeSet, Mana, OldMana);
	COLLABATTRIBUTE_REPNOTIFY(GetManaAttribute(), OldMana.GetCurrentValue(), GetMana());
}

void UCollabSpellcastAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabSpellcastAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabSpellcastAttributeSet, Mana, COND_None, REPNOTIFY_Always);
}

void UCollabSpellcastAttributeSet::ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::ClampAttributes(Attribute, NewValue);
	
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
		return;
	}

	if (Attribute == GetMaxManaAttribute())
	{
		// Stops max mana from going below 1 - CR
		NewValue = FMath::Max(NewValue, 1.f);
		return;
	}
}
