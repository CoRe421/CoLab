// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Attributes/CollabAttributeSet.h"

#include "GameplayEffectExtension.h"

void UCollabAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (OldValue != NewValue)
	{
		OnAttributeChanged_BP.Broadcast(Attribute, OldValue, NewValue);
	}
}

bool UCollabAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	PendingChangedAttributes.Add(Data.EvaluatedData.Attribute.AttributeName, Data.EvaluatedData.Attribute.GetNumericValue(this));
	return true;
}

void UCollabAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const float& OldValue = PendingChangedAttributes.FindAndRemoveChecked(Data.EvaluatedData.Attribute.AttributeName);
	const float NewValue = Data.EvaluatedData.Attribute.GetNumericValue(this);
	if (OldValue == NewValue)
	{
		return;
	}
	
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();
	
	OnAttributeChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude,
		Data.EvaluatedData.Attribute, OldValue, NewValue);
	
}
