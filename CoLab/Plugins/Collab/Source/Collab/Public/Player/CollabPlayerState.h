// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "GameplayAbilitySystem/Attributes/CollabAttributeSetBase.h"
#include "CollabPlayerState.generated.h"

class UCollabGameplayEffect;
class UCollabGameplayAbility;
class UCollabAbilitySystemComponent;
class UCollabSpellcastAttributeSet;
class UCollabHealthAttributeSet;
/**
 * 
 */
UCLASS()
class COLLAB_API ACollabPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<UCollabAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<UCollabHealthAttributeSet> HealthAttributeSet;

	// Should be added in attribute set array
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	// TObjectPtr<UCollabSpellcastAttributeSet> SpellcastAttributeSet;

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	TArray<TSoftClassPtr<UCollabGameplayAbility>> DefaultAbilities;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TSoftClassPtr<UCollabGameplayEffect> DefaultAttributeEffect;

protected:
	

public:
	ACollabPlayerState();

	UFUNCTION(BlueprintCallable, Category = "Lyra|PlayerState")
	UCollabAbilitySystemComponent* GetCollabAbilitySystemComponent() const { return AbilitySystemComponent; }
	UFUNCTION(BlueprintCallable)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};
