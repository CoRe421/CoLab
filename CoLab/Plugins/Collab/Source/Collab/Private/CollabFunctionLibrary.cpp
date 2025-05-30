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

bool UCollabFunctionLibrary::IsActorOnClient(const AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return false;
	}
	const bool bIsOnClient = Actor->IsNetMode(NM_Client);
	return bIsOnClient;
}

bool UCollabFunctionLibrary::IsActorOnServer(const AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return false;
	}
	const bool bIsOnClient = IsActorOnClient(Actor);
	const bool bHasAuthority = Actor->HasAuthority();
	return bHasAuthority && !bIsOnClient;
}

void UCollabFunctionLibrary::SortArray(TArray<UObject*>& Array, const FDynamicCallback_SortDelegate& SortDelegate)
{
	if (!SortDelegate.IsBound())
	{
		return;
	}
	
	Algo::Sort(Array, [&](const UObject* A, const UObject* B)
	{
		return SortDelegate.Execute(A, B);
	});
}

TArray<UObject*> UCollabFunctionLibrary::FilterArray(const TArray<UObject*>& Array,
	const FDynamicCallback_FilterDelegate& FilterDelegate)
{
	TArray<UObject*> NewArray;
	for (const UObject* Object : Array)
	{
		if (FilterDelegate.Execute(Object))
		{
			NewArray.Emplace(const_cast<UObject*>(Object));
		}
	}

	return NewArray;
}

bool UCollabFunctionLibrary::IsPlayingInEditor(UObject* WorldContext)
{
#if WITH_EDITOR
	if (!IsValid(WorldContext))
	{
		return false;
	}

	UWorld* World = WorldContext->GetWorld();
	if (!IsValid(World))
	{
		return false;
	}

	const bool bIsPIE = World->IsPlayInEditor();
	return bIsPIE;
#else
	return false;
#endif
}
