// Fill out your copyright notice in the Description page of Project Settings.


#include "Config/CollabConfigData.h"

void UCollabConfigData::SetNetAddressable()
{
	bIsNetAddressable = true;
}

bool UCollabConfigData::IsNameStableForNetworking() const
{
	return bIsNetAddressable || Super::IsNameStableForNetworking();
}
