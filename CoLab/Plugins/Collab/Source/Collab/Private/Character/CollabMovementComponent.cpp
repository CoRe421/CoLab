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
	if (!ensureAlways(IsValid(InASC)))
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
		MovementSet->OnAttributeChanged.AddUObject(this, &ThisClass::OnMovementAttributeChanged);

		TargetJumpHeight = MovementSet->GetJumpHeight();
		OnJumpValuesUpdated();
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
		MovementSet->OnAttributeChanged.RemoveAll(this);
	}

	MovementSet = nullptr;
	AbilitySystemComponent = nullptr;
}

void UCollabMovementComponent::OnMovementAttributeChanged(AActor* EffectInstigator, AActor* EffectCauser,
	const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, const FGameplayAttribute& Attribute, float OldValue,
	float NewValue)
{
	if (Attribute == MovementSet->GetMovementSpeedAttribute())
	{
		MaxWalkSpeed = NewValue;
	}
	else if (Attribute == MovementSet->GetJumpHeightAttribute())
	{
		TargetJumpHeight = NewValue;
		OnJumpValuesUpdated();
	}
	else if (Attribute == MovementSet->GetMassAttribute())
	{
		Mass = NewValue;
	}
	else if (Attribute == MovementSet->GetGravityScaleAttribute())
	{
		GravityScale = NewValue;
		OnJumpValuesUpdated();
	}
	else if (Attribute == MovementSet->GetAirControlAttribute())
	{
		AirControl = NewValue;
	}
	else if (Attribute == MovementSet->GetAirControlBoostMultiplierAttribute())
	{
		AirControlBoostMultiplier = NewValue;
	}
	else if (Attribute == MovementSet->GetAirControlBoostVelocityThresholdAttribute())
	{
		AirControlBoostVelocityThreshold = NewValue;
	}
	else
	{
		UE_LOG(LogCollab, Error, TEXT("Movement set attribute not handled"))
	}
}

void UCollabMovementComponent::OnJumpValuesUpdated()
{
	const AActor* Owner = AbilitySystemComponent->GetOwner();
	if (!ensureAlways(IsValid(Owner)))
	{
		return;
	}
	const UWorld* World = Owner->GetWorld();
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}
	const float Gravity = World->GetGravityZ();
	const float OwnerGravity = Gravity * GravityScale;

	// x == JumpInitialVelocity
	// JumpHeight = -x/2 * x/g
	// JumpHeight = x(-1/2g)
	// x = -2g(JumpHeight)
	JumpZVelocity = -2 * OwnerGravity * TargetJumpHeight;
}

