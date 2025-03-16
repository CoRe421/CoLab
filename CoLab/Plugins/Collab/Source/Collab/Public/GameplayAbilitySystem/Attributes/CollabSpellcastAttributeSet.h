// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "NativeGameplayTags.h"
#include "GameplayAbilitySystem/Attributes/CollabAttributeSet.h"
#include "CollabSpellcastAttributeSet.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynamicEvent_OnManaChanged, const float, NewMana, const float, OldMana);

namespace CollabSpellcastGameplayTags
{
	COLLAB_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_State_Spellcast);
}

/**
 * 
 */
UCLASS(BlueprintType)
class COLLAB_API UCollabSpellcastAttributeSet : public UCollabAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Max Mana", ReplicatedUsing = OnRep_MaxMana, meta=(AllowPrivateAccess))
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UCollabSpellcastAttributeSet, MaxMana)
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_Mana, meta=(AllowPrivateAccess))
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UCollabSpellcastAttributeSet, Mana)
	
	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);
	UFUNCTION()
	virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const override;
};
