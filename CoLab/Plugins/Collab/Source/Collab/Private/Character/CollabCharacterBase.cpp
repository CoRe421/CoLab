// Fill out your copyright notice in the Description page of Project Settings.


#include "Character\CollabCharacterBase.h"

#include "CollabGameplayTags.h"
#include "CollabLog.h"
#include "Components/CapsuleComponent.h"
#include "Character/CollabPawnExtensionComponent.h"
#include "GameplayEffectTypes.h"
#include "Character/CollabAttributeComponent.h"
#include "Character/CollabDeathComponent.h"
#include "Character/CollabMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"


// Sets default values
ACollabCharacterBase::ACollabCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCollabMovementComponent>(CharacterMovementComponentName))
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// PawnExtComponent = CreateDefaultSubobject<UCollabPawnExtensionComponent>(TEXT("PawnExtensionComponent"));

	// DeathComponent = CreateDefaultSubobject<UCollabDeathComponent>(TEXT("DeathComponent"));
	// DeathComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	// DeathComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

	// CollabMovementComponent = CreateDefaultSubobject<UCollabMovementComponent>(TEXT("CollabMovementComponent"));
	// if (ensureAlways(CollabMovementComponent))
	// {
	// 	UCapsuleComponent* FoundCapsuleComponent = GetCapsuleComponent();
	// 	if (ensure(FoundCapsuleComponent))
	// 	{
	// 		CollabMovementComponent->UpdatedComponent = FoundCapsuleComponent;
	// 	}
	// }

	PawnExtensionComponent = CreateDefaultSubobject<UCollabPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	if (ensureAlways(PawnExtensionComponent))
	{
		PawnExtensionComponent->OnAbilitySystemInit.AddUniqueDynamic(this, &ThisClass::OnAbilitySystemInitialized);
	}
	
	// AttributeComponent = CreateDefaultSubobject<UCollabAttributeComponent>(TEXT("AttributeComponent"));
}

// Called when the game starts or when spawned
void ACollabCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACollabCharacterBase::OnAbilitySystemInitialized(UCollabAbilitySystemComponent* CollabASC)
{
	
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
	if (!IsValid(PawnExtensionComponent))
	{
		return nullptr;
	}

	return PawnExtensionComponent->GetCollabAbilitySystemComponent();
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
	UCollabAbilitySystemComponent* CollabASC = GetCollabAbilitySystemComponent();
	if (IsValid(CollabASC))
	{
		if (CollabASC->GetAvatarActor() == this)
		{
			PawnExtensionComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}