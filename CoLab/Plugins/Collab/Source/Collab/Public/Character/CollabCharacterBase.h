// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "CollabPawnExtensionComponent.h"
#include "GameFramework/Character.h"
#include "CollabCharacterBase.generated.h"


class UCollabMovementComponent;
class UCollabAttributeComponent;
class UCollabAbilitySystemComponent;

UCLASS(Abstract)
class COLLAB_API ACollabCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)
	FDynamicEvent_OnAbilitySystemInit OnAbilitySystemInit;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<UCollabPawnExtensionComponent> PawnExtensionComponent;
	
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	// TObjectPtr<UCollabMovementComponent> CollabMovementComponent;
	
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	// TObjectPtr<UCollabAttributeComponent> AttributeComponent;

public:
	// Sets default values for this pawn's properties
	ACollabCharacterBase(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnAbilitySystemInitialized(UCollabAbilitySystemComponent* CollabASC);

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
};