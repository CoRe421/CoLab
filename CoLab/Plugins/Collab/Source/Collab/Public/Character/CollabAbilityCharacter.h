// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollabCharacterBase.h"
#include "CollabAbilityCharacter.generated.h"

class UGameplayEffect;
class UGameplayAbility;
class UCollabAttributeSetBase;
class UCollabAbilitySystemComponent;

UCLASS()
class COLLAB_API ACollabAbilityCharacter : public ACollabCharacterBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCollabAbilitySystemComponent> CharacterASC;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	TObjectPtr<UCollabAttributeSetBase> CharacterAttributeSet;

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	TArray<TSoftClassPtr<UGameplayAbility>> DefaultAbilities;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TSoftClassPtr<UGameplayEffect> DefaultAttributeEffect;

public:
	// Sets default values for this character's properties
	ACollabAbilityCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
