// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "CollabHeroComponent.generated.h"

struct FInputActionValue;
struct FGameplayTag;
class UInputMappingContext;
// This may be moved somewhere else
USTRUCT()
struct FInputMappingContextAndPriority
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Input", meta=(AssetBundles="Client,Server"))
	TSoftObjectPtr<UInputMappingContext> InputMapping;

	// Higher priority input mappings will be prioritized over mappings with a lower priority.
	UPROPERTY(EditAnywhere, Category="Input")
	int32 Priority = 0;
	
	/** If true, then this mapping context will be registered with the settings when this game feature action is registered. */
	UPROPERTY(EditAnywhere, Category="Input")
	bool bRegisterWithSettings = true;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COLLAB_API UCollabHeroComponent : public UPawnComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	TArray<FInputMappingContextAndPriority> DefaultInputMappings;

public:
	// Sets default values for this component's properties
	UCollabHeroComponent(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeHeroComponent();

protected:

	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookStick(const FInputActionValue& InputActionValue);
};
