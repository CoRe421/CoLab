// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CollabPlayerState.h"

#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"
#include "GameplayAbilitySystem/Attributes/CollabHealthAttributeSet.h"

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
