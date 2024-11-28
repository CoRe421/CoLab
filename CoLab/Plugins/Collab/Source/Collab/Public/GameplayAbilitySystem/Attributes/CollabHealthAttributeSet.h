// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "NativeGameplayTags.h"
#include "GameplayAbilitySystem/Attributes/CollabAttributeSet.h"
#include "CollabHealthAttributeSet.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynamicEvent_OnHealthChanged, const float, NewHealth, const float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicEvent_OnOutOfHealth);

namespace CollabHealthGameplayTags
{
	COLLAB_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_State_Damaged);
}

/**
 * 
 */
UCLASS(BlueprintType)
class COLLAB_API UCollabHealthAttributeSet : public UCollabAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, DisplayName="OnHealthChanged")
	mutable FDynamicEvent_OnHealthChanged BP_OnHealthChanged;
	mutable FNativeEvent_CollabAttributeEvent OnHealthChanged;
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, DisplayName="OnOutOfHealth")
	mutable FDynamicEvent_OnOutOfHealth BP_OnOutOfHealth;
	mutable FNativeEvent_CollabAttributeEvent OnOutOfHealth;

private:
	// Used to track when the health reaches 0.
	bool bOutOfHealth;

	// Store the health before any changes 
	float HealthBeforeAttributeChange;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Max Health", ReplicatedUsing = OnRep_MaxHealth, meta=(AllowPrivateAccess))
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UCollabHealthAttributeSet, MaxHealth)
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health, meta=(AllowPrivateAccess))
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UCollabHealthAttributeSet, Health)
	
	// Damage is a meta attribute used by the DamageExecution to calculate final damage, which then turns into -Health
	// Temporary value that only exists on the Server. Not replicated.
	UPROPERTY(BlueprintReadOnly, Category = "Damage", meta=(AllowPrivateAccess))
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UCollabHealthAttributeSet, Damage)

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
};
