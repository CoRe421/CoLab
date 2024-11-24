// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CollabHeroComponent.h"

#include "AsyncTreeDifferences.h"
#include "CollabGameplayTags.h"
#include "CollabLog.h"
#include "EnhancedInputSubsystems.h"
#include "Character/CollabPawnData.h"
#include "Input/CollabInputComponent.h"
#include "Input/CollabInputConfig.h"
#include "Player/CollabPlayerController.h"
#include "Player/CollabPlayerState.h"
#include "InputMappingContext.h"
#include "Character/CollabPawnExtensionComponent.h"
#include "GameFramework/Character.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "UserSettings/EnhancedInputUserSettings.h"


namespace CollabHero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
};

const FName UCollabHeroComponent::NAME_ActorFeatureName("Hero");

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

	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(UCollabPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	ensure(TryToChangeInitState(CollabGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UCollabHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	
	Super::EndPlay(EndPlayReason);
}

void UCollabHeroComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		UE_LOG(LogCollab, Error, TEXT("[UCollabHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}


// Called every frame
void UCollabHeroComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UCollabHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == CollabGameplayTags::InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == CollabGameplayTags::InitState_Spawned && DesiredState == CollabGameplayTags::InitState_DataAvailable)
	{
		
		AController* Controller1 = GetController<AController>();
		if (IsValid(Controller1) && IsValid(Controller1->InputComponent))
		{
			int32 test = 1;
		}
		// The player state is required.
		if (!GetPlayerState<ACollabPlayerState>())
		{
			if (IsValid(Controller1) && IsValid(Controller1->InputComponent))
			{
				int32 test = 2;
			}
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
				(Controller->PlayerState != nullptr) && \
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			ACollabPlayerController* CollabPC = GetController<ACollabPlayerController>();

			// The input component and local player is required when locally controlled.
			if (!Pawn->InputComponent || !CollabPC || !CollabPC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == CollabGameplayTags::InitState_DataAvailable && DesiredState == CollabGameplayTags::InitState_DataInitialized)
	{
		// Wait for player state and extension component
		ACollabPlayerState* CollabPS = GetPlayerState<ACollabPlayerState>();

		if (!IsValid(CollabPS))
		{
			return false;
		}

		const bool bPawnExtReady = Manager->HasFeatureReachedInitState(Pawn, UCollabPawnExtensionComponent::NAME_ActorFeatureName, CollabGameplayTags::InitState_DataInitialized);
		return bPawnExtReady;
	}
	else if (CurrentState == CollabGameplayTags::InitState_DataInitialized && DesiredState == CollabGameplayTags::InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		return true;
	}

	return false;
}

void UCollabHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState)
{
	if (CurrentState == CollabGameplayTags::InitState_DataAvailable && DesiredState == CollabGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ACollabPlayerState* CollabPS = GetPlayerState<ACollabPlayerState>();
		if (!ensure(Pawn && CollabPS))
		{
			return;
		}

		const UCollabPawnData* PawnData = nullptr;

		if (UCollabPawnExtensionComponent* PawnExtComp = UCollabPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UCollabPawnData>();

			// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
			// The ability system component and attribute sets live on the player state.
			PawnExtComp->InitializeAbilitySystem(CollabPS->GetCollabAbilitySystemComponent(), CollabPS);
		}
		
		if (ACollabPlayerController* CollabPC = GetController<ACollabPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}

		// // Hook up the delegate for all pawns, in case we spectate later
		// if (PawnData)
		// {
		// 	if (UCollabCameraComponent* CameraComponent = UCollabCameraComponent::FindCameraComponent(Pawn))
		// 	{
		// 		CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
		// 	}
		// }
	}
}

void UCollabHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UCollabPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == CollabGameplayTags::InitState_DataInitialized)
		{
			// If the extension component says all all other components are initialized, try to progress to next state
			CheckDefaultInitialization();
		}
	}
}

void UCollabHeroComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = { CollabGameplayTags::InitState_Spawned, CollabGameplayTags::InitState_DataAvailable, CollabGameplayTags::InitState_DataInitialized, CollabGameplayTags::InitState_GameplayReady };

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
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
	
	const UCollabPawnExtensionComponent* PawnExtComp = UCollabPawnExtensionComponent::FindPawnExtensionComponent(Pawn);
	if (IsValid(PawnExtComp))
	{
		const UCollabPawnData* PawnData = PawnExtComp->GetPawnData<UCollabPawnData>();
		if (IsValid(PawnData))
		{
			for (const TSoftObjectPtr<UCollabInputConfig>& InputConfig : PawnData->DefaultInputConfigs)
			{
				UCollabInputConfig* LoadedInputConfig = InputConfig.LoadSynchronous();
				if (!IsValid(LoadedInputConfig))
				{
					continue;
				}
				// The Collab Input Component has some additional functions to map Gameplay Tags to an Input Action.
				// If you want this functionality but still want to change your input component class, make it a subclass
				// of the UCollabInputComponent or modify this component accordingly.
				UCollabInputComponent* CollabIC = Cast<UCollabInputComponent>(PlayerInputComponent);
				if (ensureMsgf(CollabIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UCollabInputComponent or a subclass of it.")))
				{
					// Add the key mappings that may have been set by the player
					CollabIC->AddInputMappings(LoadedInputConfig, Subsystem);

					// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
					// be triggered directly by these input actions Triggered events. 
					TArray<uint32> BindHandles;
					CollabIC->BindAbilityActions(LoadedInputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased);

					CollabIC->BindNativeAction(LoadedInputConfig, CollabGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
					CollabIC->BindNativeAction(LoadedInputConfig, CollabGameplayTags::InputTag_Jump, ETriggerEvent::Triggered, this, &ThisClass::Input_Jump, /*bLogIfNotFound=*/ false);
					CollabIC->BindNativeAction(LoadedInputConfig, CollabGameplayTags::InputTag_Jump, ETriggerEvent::Canceled, this, &ThisClass::Input_StopJumping, /*bLogIfNotFound=*/ false);
					CollabIC->BindNativeAction(LoadedInputConfig, CollabGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
					CollabIC->BindNativeAction(LoadedInputConfig, CollabGameplayTags::InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
					// CollabIC->BindNativeAction(InputConfig, CollabGameplayTags::InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
					// CollabIC->BindNativeAction(InputConfig, CollabGameplayTags::InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
				}
			}
			
			for (const FInputMappingContextAndPriority& Mapping : PawnData->DefaultInputMappings)
			{
				UInputMappingContext* IMC = Mapping.InputMapping.LoadSynchronous();
				if (!IsValid(IMC))
				{
					continue;
				}
				
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

void UCollabHeroComponent::Input_Jump(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!IsValid(Pawn))
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(Pawn);
	if (!IsValid(Character))
	{
		return;
	}

	Character->Jump();
}

void UCollabHeroComponent::Input_StopJumping(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!IsValid(Pawn))
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(Pawn);
	if (!IsValid(Character))
	{
		return;
	}

	Character->StopJumping();
}

void UCollabHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!IsValid(Pawn))
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
	if (!IsValid(Pawn))
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

