// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "CollabActivatableWidgetStack.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynamicEvent_OnStackTransitioningChanged, UCollabActivatableWidgetStack*, WidgetStack, const bool, bIsTransitioning);

/**
 * 
 */
UCLASS(BlueprintType)
class COLLAB_API UCollabActivatableWidgetStack : public UCommonActivatableWidgetStack
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, DisplayName="On Transitioning Changed")
	FDynamicEvent_OnStackTransitioningChanged OnStackTransitioningChanged;
	
public:
	UCollabActivatableWidgetStack();

protected:
	UFUNCTION()
	void ReceiveOnTransitioningChanged(UCommonActivatableWidgetContainerBase* Widget, bool bIsTransitioning);
};
