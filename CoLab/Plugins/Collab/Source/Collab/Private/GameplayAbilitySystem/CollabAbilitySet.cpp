// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/CollabAbilitySet.h"

#include "ActiveGameplayEffectHandle.h"
#include "CollabLog.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"
#include "GameplayAbilitySystem/Abilities/CollabGameplayAbility.h"
#include "GameplayAbilitySystem/Attributes/CollabAttributeSet.h"
#include "GameplayAbilitySystem/Effects/CollabGameplayEffect.h"

void FCollabAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FCollabAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FCollabAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

void FCollabAbilitySet_GrantedHandles::TakeFromAbilitySystem(UCollabAbilitySystemComponent* CollabASC)
{
	check(CollabASC);

	if (!CollabASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			CollabASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			CollabASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		CollabASC->RemoveSpawnedAttribute(Set);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

UCollabAbilitySet::UCollabAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCollabAbilitySet::GrantToAbilitySystem(UCollabAbilitySystemComponent* CollabASC, FCollabAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(CollabASC);

	if (!CollabASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Grant the attribute sets.
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FCollabAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];
	
		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogCollab, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"), SetIndex, *GetNameSafe(this));
			continue;
		}
	
		UCollabAttributeSet* NewSet = NewObject<UCollabAttributeSet>(CollabASC->GetOwner(), SetToGrant.AttributeSet);
		CollabASC->GiveCharacterAttributeSet(NewSet);
	
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FCollabAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogCollab, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UCollabGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UCollabGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = CollabASC->GiveCharacterAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < GameplayEffects.Num(); ++EffectIndex)
	{
		const FCollabAbilitySet_GameplayEffect& EffectToGrant = GameplayEffects[EffectIndex];
		UClass* LoadedEffect = EffectToGrant.GameplayEffect.LoadSynchronous();
		if (!IsValid(LoadedEffect) || !LoadedEffect->IsChildOf(UCollabGameplayEffect::StaticClass()))
		{
			UE_LOG(LogCollab, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"), EffectIndex, *GetNameSafe(this));
			continue;
		}

		FGameplayEffectContextHandle EffectContext = CollabASC->MakeEffectContext();
		EffectContext.AddSourceObject(CollabASC->GetAvatarActor());

		FGameplayEffectSpecHandle EffectSpecHandle = CollabASC->MakeOutgoingSpec(LoadedEffect, EffectToGrant.EffectLevel, EffectContext);
		if (EffectSpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle GameplayEffectHandle = CollabASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

			if (OutGrantedHandles)
			{
				OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
			}
		}
	}
}