// Fill out your copyright notice in the Description page of Project Settings.

#define ALLOW_METADATA_MACRO
#include "Config/CollabConfigData.h"

#define STRING(s) #s
UCollabConfigData::UCollabConfigData()
{
	// This doesn't actually work at runtime, just a stupid party trick for auto adding categories to cpp properties - CR
	for (TFieldIterator<FProperty> PropIt(UCollabConfigData::StaticClass()); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;
		if (!ensureAlways(Property))
		{
			continue;
		}

		const TMap<FName, FString>* MetaData = Property->GetMetaDataMap();
		const FString* AddMetaData = MetaData->Find(STRING(META_PROPERTY));
		if (!AddMetaData)
		{
			continue;
		}

		Property->SetMetaData(TEXT("Category"), *MetaSpecifier);
	}
}
#undef STRING

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

#undef META_PROPERTY