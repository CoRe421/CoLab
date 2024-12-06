// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/CollabActivatableWidgetStack.h"

UCollabActivatableWidgetStack::UCollabActivatableWidgetStack()
{
	OnTransitioningChanged.AddUObject(this, &ThisClass::ReceiveOnTransitioningChanged);
}

void UCollabActivatableWidgetStack::ReceiveOnTransitioningChanged(UCommonActivatableWidgetContainerBase* Widget,
	bool bIsTransitioning)
{
	OnStackTransitioningChanged.Broadcast(this, bIsTransitioning);
}
