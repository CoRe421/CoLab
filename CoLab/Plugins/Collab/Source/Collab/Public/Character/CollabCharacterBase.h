// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "CollabCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicEvent_OnAbilitySystemInit, UCollabAbilitySystemComponent*, ASC);

class UCollabAbilitySystemComponent;
class UCollabPawnExtensionComponent;

UCLASS(Abstract)
class COLLAB_API ACollabCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	TWeakObjectPtr<UCollabAbilitySystemComponent> AbilitySystemComponent;

public:
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)
	FDynamicEvent_OnAbilitySystemInit OnAbilitySystemInit;

private:
	
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collab|Character", Meta = (AllowPrivateAccess = "true"))
	// TObjectPtr<UCollabDeathComponent> DeathComponent;

	// protected:
	// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	// 	TObjectPtr<UCollabPawnExtensionComponent> PawnExtComponent;

public:
	// Sets default values for this pawn's properties
	ACollabCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	virtual UCollabAbilitySystemComponent* GetCollabAbilitySystemComponent() const;
	UFUNCTION(BlueprintCallable)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void BeginDestroyCharacter(const bool bDeffered = false);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void FinishDestroyCharacter();
	
protected:
	void DisableMovementAndCollision();
	void DestroyCharacter();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnDestroyCharacter();

	void UninitAndDestroy();
	void UninitializeAbilitySystem();
};