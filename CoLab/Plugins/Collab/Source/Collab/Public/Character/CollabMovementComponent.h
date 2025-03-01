// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CollabMovementComponent.generated.h"


struct FGameplayAttribute;
struct FGameplayEffectSpec;
class UCollabMovementAttributeSet;
class UCollabAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COLLAB_API UCollabMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	// Ability system used by this component.
	UPROPERTY()
	TWeakObjectPtr<UCollabAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	UPROPERTY()
	TWeakObjectPtr<const UCollabMovementAttributeSet> MovementSet;

	UPROPERTY()
	float TargetJumpHeight = 0;

public:
	// Sets default values for this component's properties
	UCollabMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Collab|Movement")
	void InitializeWithAbilitySystem(UCollabAbilitySystemComponent* InASC);
	// Uninitialize the component, clearing any references to the ability system.
	UFUNCTION(BlueprintCallable, Category = "Collab|Movement")
	void UninitializeFromAbilitySystem();

	virtual void OnMovementAttributeChanged(AActor* EffectInstigator, AActor* EffectCauser,
		const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, const FGameplayAttribute& Attribute,
		float OldValue, float NewValue);

private:
	void OnJumpValuesUpdated();
};
