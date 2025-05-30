// Fill out your copyright notice in the Description page of Project Settings.


#include "Config/CollabConfigManager.h"

#include "CollabLog.h"
#include "Config/CollabConfigData.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"


void ACollabConfigManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACollabConfigManager, ConstructedConfigs);
}

// Sets default values
ACollabConfigManager::ACollabConfigManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bReplicateUsingRegisteredSubObjectList = true;
	bAlwaysRelevant = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComponent);
}

UCollabConfigData* ACollabConfigManager::FindOrCreateConfigData(const TSubclassOf<UCollabConfigData>& ConfigDataClass)
{
	if (!IsValid(ConfigDataClass))
	{
		return nullptr;
	}
	
	for (const TObjectPtr<UCollabConfigData>& FoundConfig : ConstructedConfigs)
	{
		if (!ensure(IsValid(FoundConfig)))
		{
			continue;
		}
		
		const UClass* FoundConfigClass = FoundConfig->GetClass();
		if (!ensure(IsValid(FoundConfigClass)))
		{
			continue;
		}

		if (FoundConfigClass == ConfigDataClass)
		{
			return FoundConfig.Get();
		}
	}

	UCollabConfigData* NewConfigData = ConstructNewConfigData(ConfigDataClass);
	if (!ensureAlways(IsValid(NewConfigData)))
	{
		return nullptr;
	}

	return NewConfigData;
}

void ACollabConfigManager::BeginPlay()
{
	Super::BeginPlay();
}

bool ACollabConfigManager::IsSupportedForNetworking() const
{
	const bool Test = Super::IsSupportedForNetworking();
	return true;
}

void ACollabConfigManager::InitializeConfigData(const TArray<TObjectPtr<UCollabConfigData>>& ConfigData)
{
	if (GetNetMode() >= NM_Client)
	{
		return;
	}

	if (!ensureAlways(ConstructedConfigs.IsEmpty()))
	{
		UE_LOG(LogCollab, Warning, TEXT("ConstructedConfigs non-empty on initialization!"))
	}
	
	ConstructedConfigs.Reset(ConfigData.Num());
	
	for (const TObjectPtr<UCollabConfigData>& InConfigData : ConfigData)
	{
		if (!ensure(IsValid(InConfigData)))
		{
			continue;
		}

		const TSubclassOf<UCollabConfigData> ConfigDataClass = InConfigData->GetClass();
		const UCollabConfigData* NewConfigData = ConstructNewConfigData(ConfigDataClass, InConfigData);
		ensureAlways(IsValid(NewConfigData));
	}
}

UCollabConfigData* ACollabConfigManager::ConstructNewConfigData(const TSubclassOf<UCollabConfigData>& ConfigDataClass,
	UCollabConfigData* ConfigDataTemplate /*= nullptr*/)
{
	if (!IsValid(ConfigDataClass))
	{
		return nullptr;
	}
	
	// const FName SafeName = MakeUniqueObjectName(this, UCollabConfigData::StaticClass(), ConfigDataClass->GetFName());
	UCollabConfigData* NewConfig = NewObject<UCollabConfigData>(this, ConfigDataClass, NAME_None, RF_NoFlags, ConfigDataTemplate);
	if (!ensureAlways(IsValid(NewConfig)))
	{
		return nullptr;
	}
	
	AddReplicatedSubObject(NewConfig);
	ConstructedConfigs.Emplace(NewConfig);
	
	return NewConfig;
}

