// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "CoLabCharacter.h"
#include "CoLabProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UTP_WeaponComponent::Fire_Implementation()
{
	if (!IsValid(AttachedCharacter) || AttachedCharacter->GetController() == nullptr)
	{
		return;
	}

	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(AttachedCharacter->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	
			// Spawn the projectile at the muzzle
			World->SpawnActor<ACoLabProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		SVR_PlayFireSound();
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = AttachedCharacter->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

bool UTP_WeaponComponent::AttachWeapon(ACoLabCharacter* TargetCharacter)
{
	// Check that the character is valid, and has no weapon component yet
	if (TargetCharacter == nullptr || TargetCharacter->GetInstanceComponents().FindItemByClass<UTP_WeaponComponent>())
	{
		return false;
	}

	// add the weapon as an instance component to the character
	UTP_WeaponComponent* NewComponent = DuplicateObject<UTP_WeaponComponent>(this, TargetCharacter);
	if (!NewComponent)
	{
		return false;
	}

	NewComponent->RegisterComponent();
	TargetCharacter->AddOwnedComponent(NewComponent);
	// Attach the weapon to the First Person Character
	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	NewComponent->AttachToComponent(TargetCharacter->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// Set up action bindings
	// if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	// {
	// 	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	// 	{
	// 		// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
	// 		Subsystem->AddMappingContext(FireMappingContext, 1);
	// 	}
	//
	// 	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	// 	{
	// 		// Fire
	// 		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
	// 	}
	// }

	return true;
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!IsValid(AttachedCharacter))
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(AttachedCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

void UTP_WeaponComponent::OnRegister()
{
	Super::OnRegister();

	AttachedCharacter = Cast<ACoLabCharacter>(GetOwner());
}


void UTP_WeaponComponent::SVR_PlayFireSound_Implementation()
{
	MUL_PlayFireSound();
}

void UTP_WeaponComponent::MUL_PlayFireSound_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(AttachedCharacter, FireSound, AttachedCharacter->GetActorLocation());
}
