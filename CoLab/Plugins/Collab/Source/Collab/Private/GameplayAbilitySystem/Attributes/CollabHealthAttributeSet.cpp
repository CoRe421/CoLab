// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Attributes/CollabHealthAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"

#include "Net/UnrealNetwork.h"

namespace CollabHealthGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_State_DamageImmune, "Gameplay.State.DamageImmune", "Target cannot take damage.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_State_Damaged, "Gameplay.State.Damaged", "Target has recently taken damage.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Gameplay_Event_DealtDamage, "Gameplay.Event.DealtDamage", "Target has been dealt damage.");
}

void UCollabHealthAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCollabHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCollabHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
}

void UCollabHealthAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabHealthAttributeSet, MaxHealth, OldMaxHealth);
	COLLABATTRIBUTE_REPNOTIFY(GetMaxHealthAttribute(), OldMaxHealth.GetCurrentValue(), GetMaxHealth());
}

void UCollabHealthAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCollabHealthAttributeSet, Health, OldHealth);
	const FGameplayAttribute& HealthAttribute = GetHealthAttribute();
	COLLABATTRIBUTE_REPNOTIFY(HealthAttribute, OldHealth.GetCurrentValue(), GetHealth());

	const float CurrentHealthValue = GetHealth();
	const float OldHealthValue = OldHealth.GetCurrentValue();
	const float EstimatedMagnitude = CurrentHealthValue - OldHealthValue;

	if ((CurrentHealthValue <= 0.0f) && !bOutOfHealth)
	{
		bOutOfHealth = true;
		
		BP_OnOutOfHealth.Broadcast();
		OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, HealthAttribute, OldHealthValue, CurrentHealthValue);
	}

	bOutOfHealth = (CurrentHealthValue <= 0.0f);
}

void UCollabHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetDamageAttribute())
	{
		const bool bCanBeDamaged = CanBeDamaged();
		if (!bCanBeDamaged)
		{
			const float CurrentDamage = GetDamage();
			NewValue = CurrentDamage;
		}
	}
	Super::PreAttributeChange(Attribute, NewValue);
}

void UCollabHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	const float OldHealth = GetHealth(); 
	const float CurrentMaxHealth = GetMaxHealth();

	const FGameplayAttribute& DamageAttribute = GetDamageAttribute();
	if (Data.EvaluatedData.Attribute == DamageAttribute)
	{
		const float LocalDamageDone = GetDamage();
		SetDamage(0.0f);

		if (LocalDamageDone > 0.0f)
		{
			const bool bCanBeDamaged = CanBeDamaged();
			// Should always be able to be damaged here, damage shouldn't be added to attribute if not - CR
			if (ensure(bCanBeDamaged))
			{
				const float ModifiedHealth = OldHealth - LocalDamageDone;
				SetHealth(FMath::Clamp(ModifiedHealth, 0.0f, CurrentMaxHealth));
			}
		}
	}
	
	const float NewHealth = GetHealth();
	const FGameplayAttribute& HealthAttribute = GetHealthAttribute();

	if ((NewHealth <= 0.0f) && !bOutOfHealth)
	{
		bOutOfHealth = true;
		
		BP_OnOutOfHealth.Broadcast();
		OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthAttribute, OldHealth, NewHealth);
	}
	
	// Check health again in case an event above changed it.
	bOutOfHealth = NewHealth <= 0.0f;
}

void UCollabHealthAttributeSet::ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::ClampAttributes(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
		return;
	}
	
	if (Attribute == GetMaxHealthAttribute())
	{
		// Stops max health from going below 1 - CR
		NewValue = FMath::Max(NewValue, 1.f);
		return;
	}
}

bool UCollabHealthAttributeSet::CanBeDamaged()
{
	const UAbilitySystemComponent* OwningASC = GetOwningAbilitySystemComponent();
	if (!ensureAlways(IsValid(OwningASC)))
	{
		return false;
	}

	const int32 ImmuneTagCount = OwningASC->GetGameplayTagCount(CollabHealthGameplayTags::Gameplay_State_DamageImmune);
	return ImmuneTagCount <= 0;
}
