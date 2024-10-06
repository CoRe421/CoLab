// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "GameplayAbilitySystem/Attributes/CollabAttributeSetBase.h"
#include "CollabPlayerState.generated.h"

class UCollabPawnData;
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

protected:
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

	// Given from the GameMode - CR
	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UCollabPawnData> PawnData;

public:
	ACollabPlayerState();

protected:
	virtual void PostInitializeComponents() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Collab|PlayerState")
	UCollabAbilitySystemComponent* GetCollabAbilitySystemComponent() const { return AbilitySystemComponent; }
	UFUNCTION(BlueprintCallable)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UCollabPawnData* InPawnData);

protected:
	UFUNCTION()
	void OnRep_PawnData();
};
