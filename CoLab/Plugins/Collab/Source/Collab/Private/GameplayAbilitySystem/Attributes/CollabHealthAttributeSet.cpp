// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Attributes/CollabHealthAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"

#include "Net/UnrealNetwork.h"

namespace CollabHealthGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_State_Damaged, "Gameplay.State.Damaged", "Target has recently taken damage.");
}

void UCollabHealthAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabHealthAttributeSet, MaxHealth, OldMaxHealth);
	COLLABATTRIBUTE_REPNOTIFY(GetMaxHealthAttribute(), OldMaxHealth.GetCurrentValue(), GetMaxHealth());
}

void UCollabHealthAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabHealthAttributeSet, Health, OldHealth);
	COLLABATTRIBUTE_REPNOTIFY(GetHealthAttribute(), OldHealth.GetCurrentValue(), GetHealth());

	const float CurrentHealth = GetHealth();
	const float EstimatedMagnitude = CurrentHealth - OldHealth.GetCurrentValue();
	
	BP_OnHealthChanged.Broadcast(CurrentHealth, MaxHealth.GetCurrentValue());
	OnHealthChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldHealth.GetCurrentValue(), CurrentHealth);

	if ((CurrentHealth <= 0.0f) && !bOutOfHealth)
	{
		BP_OnOutOfHealth.Broadcast();
		OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldHealth.GetCurrentValue(), CurrentHealth);
	}

	bOutOfHealth = (CurrentHealth <= 0.0f);
}

void UCollabHealthAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCollabHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
}

void UCollabHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UCollabHealthAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (bOutOfHealth && (GetHealth() > 0.0f))
	{
		bOutOfHealth = false;
	}
}

bool UCollabHealthAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	
	// Save the current health
	HealthBeforeAttributeChange = GetHealth();
	
	return true;
}

void UCollabHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();
	
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float LocalDamageDone = GetDamage();
		SetDamage(0.0f);

		if (LocalDamageDone > 0.0f)
		{
			const float NewHealth = GetHealth() - LocalDamageDone;
			SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
		}
	}

	// Use this function to trigger in-game reactions to attribute changes
	const float CurrentHealth = GetHealth();
	if (CurrentHealth != HealthBeforeAttributeChange)
	{
		BP_OnHealthChanged.Broadcast(CurrentHealth, GetMaxHealth());
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	if ((CurrentHealth <= 0.0f) && !bOutOfHealth)
	{
		BP_OnOutOfHealth.Broadcast();
		OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	// Check health again in case an event above changed it.
	bOutOfHealth = CurrentHealth <= 0.0f;
}

void UCollabHealthAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		// Stops max health from going below 1 - CR
		NewValue = FMath::Max(NewValue, 1.f);
	}
}
