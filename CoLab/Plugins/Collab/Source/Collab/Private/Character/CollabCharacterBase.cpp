// Fill out your copyright notice in the Description page of Project Settings.


#include "Character\CollabCharacterBase.h"

#include "CollabGameplayTags.h"
#include "CollabLog.h"
#include "Components/CapsuleComponent.h"
#include "Character/CollabPawnExtensionComponent.h"
#include "GameplayEffectTypes.h"
#include "Character/CollabDeathComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"


// Sets default values
ACollabCharacterBase::ACollabCharacterBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// PawnExtComponent = CreateDefaultSubobject<UCollabPawnExtensionComponent>(TEXT("PawnExtensionComponent"));

	// DeathComponent = CreateDefaultSubobject<UCollabDeathComponent>(TEXT("DeathComponent"));
	// DeathComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	// DeathComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);
}

// Called when the game starts or when spawned
void ACollabCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACollabCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACollabCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UCollabAbilitySystemComponent* ACollabCharacterBase::GetCollabAbilitySystemComponent() const
{
	return Cast<UCollabAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ACollabCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void ACollabCharacterBase::BeginDestroyCharacter(const bool bDeffered /*= false*/)
{
	DisableMovementAndCollision();

	if (!bDeffered)
	{
		FinishDestroyCharacter();
	}
}

void ACollabCharacterBase::FinishDestroyCharacter()
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyCharacter);
}

void ACollabCharacterBase::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UCharacterMovementComponent* MoveComp = CastChecked<UCharacterMovementComponent>(GetCharacterMovement());
	MoveComp->StopMovementImmediately();
	MoveComp->DisableMovement();
}

void ACollabCharacterBase::DestroyCharacter()
{
	OnDestroyCharacter();

	UninitAndDestroy();
}

void ACollabCharacterBase::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	UninitializeAbilitySystem();

	SetActorHiddenInGame(true);
}

void ACollabCharacterBase::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (AbilitySystemComponent->GetAvatarActor() == this)
	{
		FGameplayTagContainer AbilityTypesToIgnore;
		AbilityTypesToIgnore.AddTag(CollabGameplayTags::Ability_Behavior_SurvivesDeath);

		AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
		// AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
			AbilitySystemComponent->ClearActorInfo();
		}

		// OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}
