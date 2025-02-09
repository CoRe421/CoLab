// Fill out your copyright notice in the Description page of Project Settings.


#include "Config/CollabConfigData.h"

#include "CollabLog.h"
#include "Net/UnrealNetwork.h"

UCollabConfigData::UCollabConfigData()
{
}

bool UCollabConfigData::IsSupportedForNetworking() const
{
	const bool bResult = Super::IsSupportedForNetworking();
	return true;
}

void UCollabConfigData::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	UClass* Class = GetClass();
	const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(Class);
	if (!IsValid(BPClass))
	{
		return;
	}

	BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
}
