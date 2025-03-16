// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CollabAttributeSet.generated.h"

class UCollabAttributeSet;
// Used for initializing attribute values - CR
struct FOnCollabInitialAttributeChangeData : public FOnAttributeChangeData
{
	FOnCollabInitialAttributeChangeData(const FGameplayAttribute& InAttribute, const UCollabAttributeSet* AttributeSet);
};

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


#define COLLABATTRIBUTE_REPNOTIFY(Attribute, OldValue, NewValue) \
{ \
	OnAttributeChanged_BP.Broadcast(Attribute, OldValue, NewValue); \
}
/*	OnAttributeChanged.Broadcast(nullptr, nullptr, nullptr, NewValue - OldValue, Attribute, OldValue, NewValue); \ */

class UCollabAbilitySystemComponent;

/** 
 * Delegate used to broadcast attribute events, some of these parameters may be null on clients: 
 * @param EffectInstigator	The original instigating actor for this event
 * @param EffectCauser		The physical actor that caused the change
 * @param EffectSpec		The full effect spec for this change
 * @param EffectMagnitude	The raw magnitude, this is before clamping
 * @param OldValue			The value of the attribute before it was changed
 * @param NewValue			The value after it was changed
*/
DECLARE_MULTICAST_DELEGATE_SevenParams(FNativeEvent_CollabAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/,
	const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, const FGameplayAttribute& /*Attribute*/,
	float /*OldValue*/, float /*NewValue*/);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDynamicEvent_OnAttributeChanged, const FGameplayAttribute&, Attribute,
	const float, OldValue, const float, NewValue);

/*
 * 
 */
UCLASS(BlueprintType)
class COLLAB_API UCollabAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

// private:
// 	TMap<FString, float> PendingChangedAttributes;

public:
	// Defer to using 'UAbilitySystemComponent::GetGameplayAttributeValueChangeDelegate()' instead when working in C++
	// mutable FNativeEvent_CollabAttributeEvent OnAttributeChanged;
	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess), DisplayName="OnAttributeChanged")
	mutable FDynamicEvent_OnAttributeChanged OnAttributeChanged_BP;

// public:
// 	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
// 	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;

protected:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const {};
};
