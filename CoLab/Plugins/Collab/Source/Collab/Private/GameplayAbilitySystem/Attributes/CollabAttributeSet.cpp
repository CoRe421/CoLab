// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Attributes/CollabAttributeSet.h"

#include "CollabLog.h"
#include "GameplayEffectExtension.h"

// #include "GameplayEffectExtension.h"


FOnCollabInitialAttributeChangeData::FOnCollabInitialAttributeChangeData(const FGameplayAttribute& InAttribute,
	const UCollabAttributeSet* AttributeSet)
{
	if (!IsValid(AttributeSet))
	{
		return;
	}

	Attribute = InAttribute;
	const float AttributeValue = Attribute.GetNumericValue(AttributeSet);
	NewValue = AttributeValue;
	OldValue = AttributeValue;
	GEModData = nullptr;
}

void UCollabAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttributes(Attribute, NewValue);
}

void UCollabAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttributes(Attribute, NewValue);
}

bool UCollabAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	bool bResult = Super::PreGameplayEffectExecute(Data);

	// Calculations to prevent gameplay effect from firing if it doesn't actually change any data - CR
	// This is to prevent unnecessary attribute changes being broadcast
	const float Magnitude = Data.EvaluatedData.Magnitude;
	const FGameplayAttribute& Attribute = Data.EvaluatedData.Attribute;
	const float CurrentValue = Attribute.GetNumericValue(this);
	float NewValue = 0.f;
	switch (Data.EvaluatedData.ModifierOp)
	{
	case EGameplayModOp::Additive:
		{
			NewValue = CurrentValue + Magnitude;
		} break;
	case EGameplayModOp::Multiplicitive:
		{
			NewValue = CurrentValue * Magnitude;
		} break;
	case EGameplayModOp::Division:
		{
			NewValue = CurrentValue / Magnitude;
		} break;
	case EGameplayModOp::Override:
		{
			NewValue = Magnitude;
		} break;
		default:
		{
			UE_LOG(LogCollab, Error, TEXT("Received invalid effect operation"));
			ensureAlways(false);
		} break;
	}
	ClampAttributes(Attribute, NewValue);
	if (NewValue == CurrentValue)
	{
		return false;
	}

	return bResult;
}

// void UCollabAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
// {
// 	Super::PostAttributeChange(Attribute, OldValue, NewValue);
// 	
// 	if (OldValue != NewValue)
// 	{
// 		// OnAttributeChanged.Broadcast(nullptr, nullptr, nullptr, NewValue - OldValue, Attribute, OldValue, NewValue);
// 		OnAttributeChanged_BP.Broadcast(Attribute, OldValue, NewValue);
// 	}
// }
//
// bool UCollabAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
// {
// 	if (!Super::PreGameplayEffectExecute(Data))
// 	{
// 		return false;
// 	}
//
// 	PendingChangedAttributes.Add(Data.EvaluatedData.Attribute.AttributeName, Data.EvaluatedData.Attribute.GetNumericValue(this));
// 	return true;
// }
//
// void UCollabAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
// {
// 	Super::PostGameplayEffectExecute(Data);
//
// 	const float& OldValue = PendingChangedAttributes.FindAndRemoveChecked(Data.EvaluatedData.Attribute.AttributeName);
// 	const float NewValue = Data.EvaluatedData.Attribute.GetNumericValue(this);
// 	if (OldValue == NewValue)
// 	{
// 		return;
// 	}
// 	
// 	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
// 	AActor* Instigator = EffectContext.GetOriginalInstigator();
// 	AActor* Causer = EffectContext.GetEffectCauser();
// 	
// 	OnAttributeChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude,
// 		Data.EvaluatedData.Attribute, OldValue, NewValue);
// 	
// }