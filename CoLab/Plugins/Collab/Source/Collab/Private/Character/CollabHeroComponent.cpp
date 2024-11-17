// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CollabHeroComponent.h"

#include "CollabGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "Character/CollabPawnData.h"
#include "Input/CollabInputComponent.h"
#include "Input/CollabInputConfig.h"
#include "Player/CollabPlayerController.h"
#include "Player/CollabPlayerState.h"
#include "InputMappingContext.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"
#include "UserSettings/EnhancedInputUserSettings.h"


namespace CollabHero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
};

// Sets default values for this component's properties
UCollabHeroComponent::UCollabHeroComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCollabHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCollabHeroComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCollabHeroComponent::InitializeHeroComponent()
{
	APawn* Pawn = GetPawn<APawn>();
	// ACollabPlayerState* CollabPlayerState = GetPlayerState<ACollabPlayerState>();
	// if (!ensure(IsValid(Pawn) && IsValid(CollabPlayerState)))
	// {
	// 	return;
	// }

	const ACollabPlayerController* CollabPlayerController = GetController<ACollabPlayerController>();
	if (IsValid(CollabPlayerController))
	{
		if (IsValid(Pawn->InputComponent))
		{
			InitializePlayerInput(Pawn->InputComponent);
		}
	}
}

void UCollabHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	if (!ensureAlways(IsValid(PlayerInputComponent)))
	{
		return;
	}

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}
	
	const ACollabPlayerState* CollabPlayerState = GetPlayerState<ACollabPlayerState>();
	if (!IsValid(CollabPlayerState))
	{
		return;
	}

	const APlayerController* PlayerController = GetController<APlayerController>();
	check(PlayerController);

	const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerController->GetLocalPlayer());
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	const UCollabPawnData* PawnData = CollabPlayerState->GetPawnData<UCollabPawnData>();
	if (IsValid(PawnData))
	{
		UCollabInputConfig* InputConfig = PawnData->InputConfig.LoadSynchronous();
		if (IsValid(InputConfig))
		{
			for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
			{
				UInputMappingContext* IMC = Mapping.InputMapping.LoadSynchronous();
				if (IsValid(IMC))
				{
					if (Mapping.bRegisterWithSettings)
					{
						UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings();
						if (IsValid(Settings))
						{
							Settings->RegisterInputMappingContext(IMC);
						}
						
						FModifyContextOptions Options = {};
						Options.bIgnoreAllPressedKeysUntilRelease = false;
						// Actually add the config to the local player							
						Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
					}
				}
			}

			// The Collab Input Component has some additional functions to map Gameplay Tags to an Input Action.
			// If you want this functionality but still want to change your input component class, make it a subclass
			// of the UCollabInputComponent or modify this component accordingly.
			UCollabInputComponent* CollabIC = Cast<UCollabInputComponent>(PlayerInputComponent);
			if (ensureMsgf(CollabIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UCollabInputComponent or a subclass of it.")))
			{
				// Add the key mappings that may have been set by the player
				CollabIC->AddInputMappings(InputConfig, Subsystem);

				// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
				// be triggered directly by these input actions Triggered events. 
				TArray<uint32> BindHandles;
				CollabIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased);

				CollabIC->BindNativeAction(InputConfig, CollabGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
				CollabIC->BindNativeAction(InputConfig, CollabGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
				CollabIC->BindNativeAction(InputConfig, CollabGameplayTags::InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
				// CollabIC->BindNativeAction(InputConfig, CollabGameplayTags::InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
				// CollabIC->BindNativeAction(InputConfig, CollabGameplayTags::InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
			}
		}
	}
	
	// if (ensure(!bReadyToBindInputs))
	// {
	// 	bReadyToBindInputs = true;
	// }
 //
	// UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	// UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UCollabHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	const ACollabPlayerState* CollabPlayerState = GetPlayerState<ACollabPlayerState>();
	if (!IsValid(CollabPlayerState))
	{
		return;
	}

	UCollabAbilitySystemComponent* CollabASC = CollabPlayerState->GetCollabAbilitySystemComponent();
	if (!IsValid(CollabASC))
	{
		return;
	}
	
	CollabASC->AbilityInputPressed(InputTag);
}

void UCollabHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const ACollabPlayerState* CollabPlayerState = GetPlayerState<ACollabPlayerState>();
	if (!IsValid(CollabPlayerState))
	{
		return;
	}

	UCollabAbilitySystemComponent* CollabASC = CollabPlayerState->GetCollabAbilitySystemComponent();
	if (!IsValid(CollabASC))
	{
		return;
	}

	CollabASC->AbilityInputReleased(InputTag);
}

void UCollabHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;
	
	if (!IsValid(Controller))
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

	if (Value.X != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		Pawn->AddMovementInput(MovementDirection, Value.X);
	}

	if (Value.Y != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		Pawn->AddMovementInput(MovementDirection, Value.Y);
	}
}

void UCollabHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void UCollabHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	const UWorld* World = GetWorld();
	check(World);

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * CollabHero::LookYawRate * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y * CollabHero::LookPitchRate * World->GetDeltaSeconds());
	}
}

