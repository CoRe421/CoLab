// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/CollabGameData.h"

const TMap<FGameplayTag, TSoftClassPtr<UCollabGameplayEffect>>& UCollabGameData::GetGlobalTagEffects() const
{
	return GlobalTagEffects;
}
