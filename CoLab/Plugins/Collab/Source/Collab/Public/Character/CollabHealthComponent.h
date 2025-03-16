// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkComponent.h"
#include "CollabHealthComponent.generated.h"

struct FOnAttributeChangeData;
struct FGameplayAttribute;
struct FGameplayEffectSpec;
class UCollabHealthAttributeSet;
class UCollabAbilitySystemComponent;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COLLAB_API UCollabHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

protected:

	// Ability system used by this component.
	UPROPERTY()
	TWeakObjectPtr<UCollabAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	UPROPERTY()
	TWeakObjectPtr<const UCollabHealthAttributeSet> HealthSet;

public:

	UCollabHealthComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the health component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Collab|Health")
	static UCollabHealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UCollabHealthComponent>() : nullptr); }

	// Initialize the component using an ability system component.
	UFUNCTION(BlueprintCallable, Category = "Collab|Health")
	void InitializeWithAbilitySystem(UCollabAbilitySystemComponent* InASC);

	// Uninitialize the component, clearing any references to the ability system.
	UFUNCTION(BlueprintCallable, Category = "Collab|Health")
	void UninitializeFromAbilitySystem();

	// // Returns the current health value.
	// UFUNCTION(BlueprintCallable, Category = "Collab|Health")
	// float GetHealth() const;
	//
	// // Returns the current maximum health value.
	// UFUNCTION(BlueprintCallable, Category = "Collab|Health")
	// float GetMaxHealth() const;

	// // Returns the current health in the range [0.0, 1.0].
	// UFUNCTION(BlueprintCallable, Category = "Collab|Health")
	// float GetHealthNormalized() const;

	// Applies enough damage to kill the owner.
	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);

public:

	// // Delegate fired when the health value has changed. This is called on the client but the instigator may not be valid
	// UPROPERTY(BlueprintAssignable)
	// FCollabHealth_AttributeChanged OnHealthChanged;
	//
	// // Delegate fired when the max health value has changed. This is called on the client but the instigator may not be valid
	// UPROPERTY(BlueprintAssignable)
	// FCollabHealth_AttributeChanged OnMaxHealthChanged;

protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	virtual void HandleDamageChanged(const FOnAttributeChangeData& Data);
	// virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,
		const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, const FGameplayAttribute& Attribute,
		float OldValue, float NewValue);

private:
	void BroadcastDamaged(const AActor* DamageInstigator, const AActor* DamageCauser,
		const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude);
};
