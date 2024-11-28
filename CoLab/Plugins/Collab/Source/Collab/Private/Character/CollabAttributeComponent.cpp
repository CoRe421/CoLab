// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CollabAttributeComponent.h"

#include "CollabLog.h"
#include "Character/CollabCharacterBase.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"


// Sets default values for this component's properties
UCollabAttributeComponent::UCollabAttributeComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	// ...
}

void UCollabAttributeComponent::InitializeWithAbilitySystem(UCollabAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent.IsValid())
	{
		UE_LOG(LogCollab, Error, TEXT("CollabHealthComponent: Health component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent.IsValid())
	{
		UE_LOG(LogCollab, Error, TEXT("CollabHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent->OnAttributeChanged_BP.AddUniqueDynamic(this, &ThisClass::OnAttributeChanged);

	for (const TTuple<TSoftClassPtr<UCollabAttributeSet>, FObjectCallbackContainer>& Pair : AttributeChangedCallbacks)
	{
		UClass* AttributeSetClass = Pair.Key.LoadSynchronous();
		if (!IsValid(AttributeSetClass))
		{
			continue;
		}

		TArray<FGameplayAttribute> Attributes;
		UAttributeSet::GetAttributesFromSetClass(AttributeSetClass, Attributes);

		for (const FGameplayAttribute& Attribute : Attributes)
		{
			const float CurrentValue = AbilitySystemComponent->GetNumericAttribute(Attribute);
			OnAttributeChanged_BP.Broadcast(Attribute, CurrentValue, CurrentValue);
			
			for (const TTuple<TWeakObjectPtr<const UObject>, FDynamicCallback_OnAttributeChanged>& CallbackPair : Pair.Value.ObjectCallbacks)
			{
				if (!CallbackPair.Key.IsValid())
				{
					continue;
				}

				CallbackPair.Value.ExecuteIfBound(Attribute, CurrentValue, CurrentValue);
			}
		}
	}
}

void UCollabAttributeComponent::UninitializeFromAbilitySystem()
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->OnAttributeChanged_BP.RemoveAll(this);
	}
	
	AbilitySystemComponent = nullptr;
}


// Called when the game starts
void UCollabAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UCollabAttributeComponent::OnRegister()
{
	Super::OnRegister();
	
	AActor* Owner = GetOwner();
	check(Owner);

	ACollabCharacterBase* CollabCharacterOwner = Cast<ACollabCharacterBase>(Owner);
	if (IsValid(CollabCharacterOwner))
	{
		CollabCharacterOwner->OnAbilitySystemInit.AddUniqueDynamic(this, &ThisClass::InitializeWithAbilitySystem);
	}
}

void UCollabAttributeComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();

	Super::OnUnregister();
}


// Called every frame
void UCollabAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCollabAttributeComponent::RegisterAttributeChangedCallback(const UObject* Owner,
	const TSoftClassPtr<UCollabAttributeSet> AttributeSet, const FDynamicCallback_OnAttributeChanged& Callback)
{
	// If replicating
	// FAttributeCallbackContainer Container;
	// if (!ensureAlways(!FindOrAddAttributeContainer(AttributeSet, Owner, Container)))
	// {
	// 	return;
	// }
	
	FObjectCallbackContainer& FoundCallbackContainer = AttributeChangedCallbacks.FindOrAdd(AttributeSet);
	FDynamicCallback_OnAttributeChanged* FoundCallback = FoundCallbackContainer.ObjectCallbacks.Find(Owner);
	if (!ensureAlways(!FoundCallback))
	{
		return;
	}

	FoundCallbackContainer.ObjectCallbacks.Emplace(Owner, Callback);
}

void UCollabAttributeComponent::ClearCallbackForAttribute(const UObject* Owner,
	const TSoftClassPtr<UCollabAttributeSet> AttributeSet)
{
	// If replicating
	// FAttributeCallbackContainer Container;
	// if (!FindOrAddAttributeContainer(AttributeSet, Owner, Container))
	// {
	// 	return;
	// }
	
	FObjectCallbackContainer& FoundCallbackContainer = AttributeChangedCallbacks.FindOrAdd(AttributeSet);
	FoundCallbackContainer.ObjectCallbacks.Remove(Owner);
}

void UCollabAttributeComponent::OnAttributeChanged(const FGameplayAttribute& Attribute, const float OldValue,
                                                   const float NewValue)
{
	OnAttributeChanged_BP.Broadcast(Attribute, OldValue, NewValue);

	const UClass* AttributeClass = Attribute.GetAttributeSetClass();

	// If replicating
	// FAttributeCallbackContainer Container;
	// if (!FindOrAddAttributeContainer(AttributeClass, nullptr, Container))
	// {
	// 	return;
	// }
	
	FObjectCallbackContainer& FoundCallbackContainer = AttributeChangedCallbacks.FindOrAdd(AttributeClass);
	for (const auto& Pair : FoundCallbackContainer.ObjectCallbacks)
	{
		if (!Pair.Key.IsValid())
		{
			continue;
		}

		Pair.Value.ExecuteIfBound(Attribute, OldValue, NewValue);
	}
}

// If replicating
// bool UCollabAttributeComponent::FindOrAddAttributeContainer(const TSoftClassPtr<UCollabAttributeSet>& AttributeSet,
// 	const UObject* Owner, FAttributeCallbackContainer& OutContainer)
// {
// 	for (FAttributeCallbackContainer& AttributeSetContainer : AttributeChangedCallbacks)
// 	{
// 		if (AttributeSetContainer.AttributeSet != AttributeSet)
// 		{
// 			continue;
// 		}
//
// 		OutContainer = AttributeSetContainer;
//
// 		if (!Owner)
// 		{
// 			return true;
// 		}
// 		
// 		FDynamicCallback_OnAttributeChanged* FoundCallback = AttributeSetContainer.ObjectCallbacks.Find(Owner);
// 		return FoundCallback != nullptr;
// 	}
//
// 	OutContainer = AttributeChangedCallbacks.Emplace_GetRef(AttributeSet);
// 	return false;
// }

