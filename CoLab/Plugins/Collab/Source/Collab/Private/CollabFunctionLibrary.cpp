// Fill out your copyright notice in the Description page of Project Settings.


#include "CollabFunctionLibrary.h"
#include "GameplayAbilitySystem/Attributes/CollabAttributeSet.h"

#include "AbilitySystemComponent.h"

const UCollabAttributeSet* UCollabFunctionLibrary::GetCollabAttributeSet(
	const UAbilitySystemComponent* AbilitySystemComponent,
	const TSubclassOf<UCollabAttributeSet> AttributeSet)
{
	if (!IsValid(AbilitySystemComponent))
	{
		return nullptr;
	}

	const UAttributeSet* FoundAttributeSet = AbilitySystemComponent->GetAttributeSet(AttributeSet);
	return Cast<UCollabAttributeSet>(FoundAttributeSet);
}

UObject* UCollabFunctionLibrary::ConstructObjectFromClassWithTemplate(UObject* Outer, const TSubclassOf<UObject> Class,
	UObject* Template)
{
	if (!IsValid(Outer) || !IsValid(Class))
	{
		return nullptr;
	}

	UObject* CreatedObject = NewObject<UObject>(Outer, Class, NAME_None, RF_NoFlags, Template);
	return CreatedObject;
}
