// Fill out your copyright notice in the Description page of Project Settings.


#include "CollabSettings.h"

UCollabSettings::UCollabSettings()
{
	GlobalFrameRateLimit = 90.f;
}

float UCollabSettings::GetGlobalFrameRateLimit()
{
	return GlobalFrameRateLimit;
}

void UCollabSettings::SetGlobalFrameRateLimit(const float FrameRateLimit)
{
	GlobalFrameRateLimit = FrameRateLimit;
}
