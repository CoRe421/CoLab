// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"
#include "GameplayAbilitySystem/Attributes/CollabAttributeSetBase.h"
#include "CollabCharacterBase.generated.h"

class UCollabPawnExtensionComponent;

UCLASS(Abstract)
class COLLAB_API ACollabCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCollabPawnExtensionComponent> PawnExtComponent;

public:
	// Sets default values for this pawn's properties
	ACollabCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GiveDefaultAbilities();
	void InitDefaultAttributes();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	virtual UCollabAbilitySystemComponent* GetCollabAbilitySystemComponent() const;
	UFUNCTION(BlueprintCallable)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};
