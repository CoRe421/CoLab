// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "NativeGameplayTags.h"

namespace CollabGameplayTags
{
	COLLAB_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
	
	COLLAB_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);
	
	// Used as the trigger for the Death GameplayAbility
	COLLAB_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);

	// Used as the status of the individual characters
	// Status.Death is the parent tag to check for death, while dying/dead are for the specifics of the death process
	COLLAB_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
	COLLAB_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
	COLLAB_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);
}
