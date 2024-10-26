// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CollabAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class UCollabAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDynamicEvent_OnAttributeChanged, const FGameplayAttribute&, Attribute,
                                             const float, OldValue, const float, NewValue);

/*
 * 
 */
UCLASS(BlueprintType)
class COLLAB_API UCollabAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess))
	mutable FDynamicEvent_OnAttributeChanged OnAttributeChanged;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetupBindings(const UCollabAbilitySystemComponent* AbilitySystemComponent);
	virtual void SetupBindings_Implementation(UCollabAbilitySystemComponent* AbilitySystemComponent) {};

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
};
