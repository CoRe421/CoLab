// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CollabMovementComponent.h"

#include "CollabLog.h"
#include "Character/CollabCharacterBase.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"
#include "GameplayAbilitySystem/Attributes/CollabMovementAttributeSet.h"


// Sets default values for this component's properties
UCollabMovementComponent::UCollabMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCollabMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UCollabMovementComponent::OnRegister()
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

void UCollabMovementComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();
	
	Super::OnUnregister();
}


// Called every frame
void UCollabMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCollabMovementComponent::InitializeWithAbilitySystem(UCollabAbilitySystemComponent* InASC)
{
	if (!ensureAlways(IsValid(CollabASC)))
	{
		return;
	}
	
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent.IsValid())
	{
		UE_LOG(LogCollab, Error, TEXT("CollabMovementComponent: Movement component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent.IsValid())
	{
		UE_LOG(LogCollab, Error, TEXT("CollabMovementComponent: Cannot initialize movement component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	MovementSet = AbilitySystemComponent->GetSet<UCollabMovementAttributeSet>();
	if (MovementSet.IsValid())
	{
		MovementSet->OnAttributeChanged
	}
	else
	{
		UE_LOG(LogCollab, Log, TEXT("CollabMovementComponent: No movement set found on owner [%s]."), *GetNameSafe(Owner));
	}
}

void UCollabMovementComponent::UninitializeFromAbilitySystem()
{
	if (MovementSet.IsValid())
	{
		// HealthSet->OnHealthChanged.RemoveAll(this);
		// // HealthSet->OnMaxHealthChanged.RemoveAll(this);
		// HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	MovementSet = nullptr;
	AbilitySystemComponent = nullptr;
}

