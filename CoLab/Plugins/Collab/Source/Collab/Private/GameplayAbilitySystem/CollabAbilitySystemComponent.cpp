// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"

#include "GameplayAbilitySystem/Abilities/CollabGameplayAbility.h"


// Sets default values for this component's properties
UCollabAbilitySystemComponent::UCollabAbilitySystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCollabAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	// ExecutePeriodicEffect()
}

void UCollabAbilitySystemComponent::OnRegister()
{
	Super::OnRegister();
}


// Called every frame
void UCollabAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, 
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCollabAbilitySystemComponent::AbilityInputPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	
	ABILITYLIST_SCOPE_LOCK();

	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (!Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			return;
		}

		const EGameplayAbilityInstancingPolicy::Type InstancePolicy = Spec.Ability->GetInstancingPolicy();
		if (!Spec.IsActive() || InstancePolicy == EGameplayAbilityInstancingPolicy::InstancedPerExecution)
		{
			TryActivateAbility(Spec.Handle);
		}
		else
		{
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle,
				Spec.ActivationInfo.GetActivationPredictionKey());
		}
	}
}

void UCollabAbilitySystemComponent::AbilityInputReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	
	ABILITYLIST_SCOPE_LOCK();

	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (!Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			return;
		}
		
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle,
			Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UCollabAbilitySystemComponent::GiveCharacterAttributeSet(UCollabAttributeSet* AttributeSet)
{
	if (!IsValid(AttributeSet))
	{
		return;
	}

	AttributeSet->OnAttributeChanged_BP.AddUniqueDynamic(this, &ThisClass::OnAttributeChanged);
	Client_OnAttributeSetAdded(AttributeSet);
	
	AddAttributeSetSubobject(AttributeSet);
}

FGameplayAbilitySpecHandle UCollabAbilitySystemComponent::GiveCharacterAbility(FGameplayAbilitySpec& AbilitySpec)
{
	const UCollabGameplayAbility* CollabAbility = Cast<UCollabGameplayAbility>(AbilitySpec.Ability);
	if (IsValid(CollabAbility))
	{
		AbilitySpec.DynamicAbilityTags.AddTag(CollabAbility->InputTag);
	}

	return GiveAbility(AbilitySpec);
}

void UCollabAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	
	if (!IsOwnerActorAuthoritative())
	{
		return;
	}

	TArray<FGameplayAttribute> GameplayAttributes;
	GetAllAttributes(GameplayAttributes);
	TSet<UClass*> FoundAttributeSets;
	for (const FGameplayAttribute& Attribute : GameplayAttributes)
	{
		UClass* AttributeSetClass = Attribute.GetAttributeSetClass();
		if (!IsValid(AttributeSetClass) || FoundAttributeSets.Contains(AttributeSetClass))
		{
			continue;
		}
		FoundAttributeSets.Emplace(AttributeSetClass);
		const UCollabAttributeSet* AttributeSet = Cast<UCollabAttributeSet>(GetAttributeSet(AttributeSetClass));
		if (!IsValid(AttributeSet))
		{
			continue;
		}

		Client_OnAttributeSetAdded(AttributeSet);
	}
}

void UCollabAbilitySystemComponent::OnAttributeChanged(const FGameplayAttribute& Attribute, const float OldValue,
                                                       const float NewValue)
{
	OnAttributeChanged_BP.Broadcast(Attribute, OldValue, NewValue);
}

void UCollabAbilitySystemComponent::Client_OnAttributeSetAdded_Implementation(const UCollabAttributeSet* AttributeSet)
{
	if (!IsValid(AttributeSet))
	{
		return;
	}

	AttributeSet->OnAttributeChanged_BP.AddUniqueDynamic(this, &ThisClass::OnAttributeChanged);
}

// void UCollabAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
// {
// 	if (InputTag.IsValid())
// 	{
// 		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
// 		{
// 			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
// 			{
// 				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
// 				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
// 			}
// 		}
// 	}
// }
//
// void UCollabAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
// {
// 	if (InputTag.IsValid())
// 	{
// 		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
// 		{
// 			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
// 			{
// 				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
// 				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
// 			}
// 		}
// 	}
// }
