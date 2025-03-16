// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CollabMovementComponent.generated.h"


struct FOnAttributeChangeData;
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

	float TargetJumpHeight;
	float AirAcceleration;
	float MaxAirSpeed;
	bool bAllowSlidingWhileMoving;
	float LandingFrictionGracePeriod;

	TOptional<FDateTime> LastLandedTime;

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

	virtual float GetMaxAcceleration() const override;

	virtual void PhysWalking(float deltaTime, int32 Iterations) override;
	virtual void PhysFalling(float deltaTime, int32 Iterations) override;
	
	virtual FVector ComputeSlideVector(const FVector& Delta, const float Time, const FVector& Normal, const FHitResult& Hit) const override;

	virtual void ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration) override;
	virtual void SetPostLandedPhysics(const FHitResult& Hit) override;

private:
	void OnJumpValuesUpdated();

	void Phys_Internal(float DeltaTime);

	void OnMovementSpeedChanged( const FOnAttributeChangeData& Data);
	void OnGroundAccelerationChanged( const FOnAttributeChangeData& Data);
	void OnGroundFrictionChanged( const FOnAttributeChangeData& Data);
	void OnAllowSlidingWhileMovingChanged( const FOnAttributeChangeData& Data);
	void OnLandingFrictionGracePeriodChanged( const FOnAttributeChangeData& Data);
	void OnJumpHeightChanged( const FOnAttributeChangeData& Data);
	void OnMassChanged( const FOnAttributeChangeData& Data);
	void OnGravityScaleChanged( const FOnAttributeChangeData& Data);
	void OnAirControlChanged( const FOnAttributeChangeData& Data);
	void OnAirSpeedChanged( const FOnAttributeChangeData& Data);
	void OnAirAccelerationChanged( const FOnAttributeChangeData& Data);
	void OnAirControlBoostMultiplierChanged( const FOnAttributeChangeData& Data);
	void OnAirControlBoostVelocityThresholdChanged( const FOnAttributeChangeData& Data);
	void OnFallingLateralFrictionChanged( const FOnAttributeChangeData& Data);
};
