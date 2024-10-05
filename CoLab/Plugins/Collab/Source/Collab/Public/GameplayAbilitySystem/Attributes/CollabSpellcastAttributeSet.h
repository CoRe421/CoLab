// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "CollabAttributeSetBase.h"
#include "CollabSpellcastAttributeSet.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class COLLAB_API UCollabSpellcastAttributeSet : public UCollabAttributeSetBase
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "Max Mana", ReplicatedUsing = OnRep_MaxMana, meta=(AllowPrivateAccess))
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UCollabSpellcastAttributeSet, MaxMana)
	UPROPERTY(BlueprintReadOnly, Category = "Current Mana", ReplicatedUsing = OnRep_CurrentMana, meta=(AllowPrivateAccess))
	FGameplayAttributeData CurrentMana;
	ATTRIBUTE_ACCESSORS(UCollabSpellcastAttributeSet, CurrentMana)
	
	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);
	UFUNCTION()
	virtual void OnRep_CurrentMana(const FGameplayAttributeData& OldCurrentMana);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
