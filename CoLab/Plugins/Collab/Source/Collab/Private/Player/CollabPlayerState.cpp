// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CollabPlayerState.h"

#include "CollabLog.h"
#include "Character/CollabPawnData.h"
#include "GameplayAbilitySystem/CollabAbilitySet.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"
#include "GameplayAbilitySystem/Attributes/CollabHealthAttributeSet.h"
#include "Net/UnrealNetwork.h"

ACollabPlayerState::ACollabPlayerState()
{
	// Default is too low and cause perceived lag - CR
	NetUpdateFrequency = 100.f;
	
	AbilitySystemComponent = CreateDefaultSubobject<UCollabAbilitySystemComponent>(TEXT("CollabPlayerStateASC"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	HealthAttributeSet = CreateDefaultSubobject<UCollabHealthAttributeSet>("HealthAttributeSet");
}

UAbilitySystemComponent* ACollabPlayerState::GetAbilitySystemComponent() const
{
	return GetCollabAbilitySystemComponent();
}

void ACollabPlayerState::SetPawnData(const UCollabPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogCollab, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	for (const UCollabAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}
	
	ForceNetUpdate();
}

void ACollabPlayerState::OnRep_PawnData()
{
}

void ACollabPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
}
