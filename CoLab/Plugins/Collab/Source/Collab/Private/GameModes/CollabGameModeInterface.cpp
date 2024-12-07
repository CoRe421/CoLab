// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/CollabGameModeInterface.h"


// Add default functionality here for any ICollabGameModeInterface functions that are not pure virtual.
void ICollabGameModeInterface::BindOnPlayerListUpdated_Implementation(const FDynamicCallback_OnPlayerListUpdated& Event)
{
	if (!Event.IsBound())
	{
		return;
	}

	OnPlayerListUpdated.AddLambda([=]()
	{
		Event.Execute();
	});
}

void ICollabGameModeInterface::BroadcastPlayerListUpdated_Implementation()
{
	OnPlayerListUpdated.Broadcast();
}
