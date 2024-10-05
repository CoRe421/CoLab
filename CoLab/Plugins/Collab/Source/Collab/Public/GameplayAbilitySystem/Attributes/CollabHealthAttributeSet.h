// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "CollabAttributeSetBase.h"
#include "CollabHealthAttributeSet.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class COLLAB_API UCollabHealthAttributeSet : public UCollabAttributeSetBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Max Health", ReplicatedUsing = OnRep_MaxHealth, meta=(AllowPrivateAccess))
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UCollabHealthAttributeSet, MaxHealth)
	UPROPERTY(BlueprintReadOnly, Category = "Current Health", ReplicatedUsing = OnRep_CurrentHealth, meta=(AllowPrivateAccess))
	FGameplayAttributeData CurrentHealth;
	ATTRIBUTE_ACCESSORS(UCollabHealthAttributeSet, CurrentHealth)
	
	// Damage is a meta attribute used by the DamageExecution to calculate final damage, which then turns into -Health
	// Temporary value that only exists on the Server. Not replicated.
	UPROPERTY(BlueprintReadOnly, Category = "Damage", meta=(AllowPrivateAccess))
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UCollabHealthAttributeSet, Damage)

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	UFUNCTION()
	virtual void OnRep_CurrentHealth(const FGameplayAttributeData& OldCurrentHealth);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
};
