// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CollabPlayerState.h"

#include "CollabLog.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "Character/CollabPawnData.h"
#include "Character/CollabPawnExtensionComponent.h"
#include "GameModes/CollabGameMode.h"
#include "GameplayAbilitySystem/CollabAbilitySet.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"
#include "GameplayAbilitySystem/Attributes/CollabHealthAttributeSet.h"
#include "Input/CollabInputComponent.h"
#include "Input/CollabInputConfig.h"
#include "Net/UnrealNetwork.h"

ACollabPlayerState::ACollabPlayerState()
{
	// Default is too low and cause perceived lag - CR
	NetUpdateFrequency = 100.f;
	
	AbilitySystemComponent = CreateDefaultSubobject<UCollabAbilitySystemComponent>(TEXT("CollabPlayerStateASC"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void ACollabPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(IsValid(AbilitySystemComponent));
	
	if (ACollabGameMode* CollabGameMode = GetWorld()->GetAuthGameMode<ACollabGameMode>())
	{
		if (const UCollabPawnData* NewPawnData = CollabGameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
		else
		{
			UE_LOG(LogCollab, Error, TEXT("ACollabPlayerState::PostInitializeComponents(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
		}
	}
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

	if (HasAuthority())
	{
		for (const TSoftObjectPtr<UCollabAbilitySet> AbilitySet : PawnData->DefaultAbilitySets)
		{
			const UCollabAbilitySet* LoadedAbilitySet = AbilitySet.LoadSynchronous();
			if (!IsValid(LoadedAbilitySet))
			{
				continue;
			}
			
			AbilitySet->GrantToAbilitySystem(AbilitySystemComponent, nullptr, this);
		}
	}
	
	ForceNetUpdate();
}

// void ACollabPlayerState::ApplyDefaultGameplayEffects()
// {
// 	if (!HasAuthority())
// 	{
// 		return;
// 	}
// 	
// 	for (const TSoftObjectPtr<UCollabAbilitySet> AbilitySet : PawnData->DefaultAbilitySets)
// 	{
// 		const UCollabAbilitySet* LoadedAbilitySet = AbilitySet.LoadSynchronous();
// 		if (!IsValid(LoadedAbilitySet))
// 		{
// 			continue;
// 		}
// 		
// 		AbilitySet->ApplyGameplayEffects(AbilitySystemComponent, nullptr);
// 	}
// 	
// 	ForceNetUpdate();
// }

void ACollabPlayerState::OnRep_PawnData()
{
}

void ACollabPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	if (UCollabPawnExtensionComponent* PawnExtComp = UCollabPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	{
		PawnExtComp->CheckDefaultInitialization();
	}
}

void ACollabPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
}
