// Fill out your copyright notice in the Description page of Project Settings.


// #include "Input/CollabInputConfig.h"
//
// #include "CollabLog.h"
//
// UCollabInputConfig::UCollabInputConfig(const FObjectInitializer& ObjectInitializer)
// {
// }
//
// const UInputAction* UCollabInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
// {
// 	for (const FCollabInputAction& Action : NativeInputActions)
// 	{
// 		if (Action.InputAction && (Action.InputTag == InputTag))
// 		{
// 			return Action.InputAction;
// 		}
// 	}
//
// 	if (bLogNotFound)
// 	{
// 		UE_LOG(LogCollab, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
// 	}
//
// 	return nullptr;
// }
//
// const UInputAction* UCollabInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
// {
// 	for (const FCollabInputAction& Action : AbilityInputActions)
// 	{
// 		if (Action.InputAction && (Action.InputTag == InputTag))
// 		{
// 			return Action.InputAction;
// 		}
// 	}
//
// 	if (bLogNotFound)
// 	{
// 		UE_LOG(LogCollab, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
// 	}
//
// 	return nullptr;
// }
