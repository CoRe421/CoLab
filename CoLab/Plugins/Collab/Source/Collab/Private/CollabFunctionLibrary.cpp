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
