// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CollabReplicatedComponentInterface.h"


// Add default functionality here for any ICollabReplicatedComponent functions that are not pure virtual.
void ICollabReplicatedComponentInterface::DestroyComponent_Implementation()
{
	UActorComponent* ThisComponent = Cast<UActorComponent>(this);
	if (!ensureAlways(IsValid(ThisComponent)))
	{
		return;
	}

	ThisComponent->DestroyComponent();
}
