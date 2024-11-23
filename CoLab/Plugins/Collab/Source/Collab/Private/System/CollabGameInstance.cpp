// Fill out your copyright notice in the Description page of Project Settings.


#include "System/CollabGameInstance.h"

#include "CollabGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"

void UCollabGameInstance::Init()
{
	Super::Init();

	// Register our custom init states
	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);

	if (ensure(ComponentManager))
	{
		ComponentManager->RegisterInitState(CollabGameplayTags::InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(CollabGameplayTags::InitState_DataAvailable, false, CollabGameplayTags::InitState_Spawned);
		ComponentManager->RegisterInitState(CollabGameplayTags::InitState_DataInitialized, false, CollabGameplayTags::InitState_DataAvailable);
		ComponentManager->RegisterInitState(CollabGameplayTags::InitState_GameplayReady, false, CollabGameplayTags::InitState_DataInitialized);
	}
}
