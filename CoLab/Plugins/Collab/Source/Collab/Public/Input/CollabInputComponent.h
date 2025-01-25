// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollabInputConfig.h"
#include "EnhancedInputComponent.h"
#include "CollabInputComponent.generated.h"


class UEnhancedInputLocalPlayerSubsystem;
class UCollabInputConfig;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COLLAB_API UCollabInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCollabInputComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


	template<class UserClass, typename TriggeredFuncType>
	void BindAbilityActions(UCollabInputConfig* InputConfig, UserClass* Object, TriggeredFuncType TriggeredFunc);

	template<class UserClass, typename FuncType>
	void BindNativeAction(const UCollabInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);

	
	void AddInputMappings(const UCollabInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	void RemoveInputMappings(const UCollabInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
};

template <class UserClass, typename TriggeredFuncType>
void UCollabInputComponent::BindAbilityActions(UCollabInputConfig* InputConfig, UserClass* Object,
	TriggeredFuncType TriggeredFunc)
{
	if (!ensureAlways(IsValid(InputConfig)))
	{
		return;
	}

	for (const FCollabInputAction& Action : InputConfig->AbilityInputActions)
	{
		const UInputAction* InputAction = Action.InputAction.LoadSynchronous();
		if (IsValid(InputAction) && Action.InputAction.IsValid())
		{
			if (TriggeredFunc)
			{
				BindAction(InputAction, ETriggerEvent::Triggered, Object, TriggeredFunc, Action.InputTag);
			}
		}
	}
}

template <class UserClass, typename FuncType>
void UCollabInputComponent::BindNativeAction(const UCollabInputConfig* InputConfig, const FGameplayTag& InputTag,
	ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}
