// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CollabConfigData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class COLLAB_API UCollabConfigData : public UPrimaryDataAsset
{
	GENERATED_BODY()

private:
	bool bIsNetAddressable = false;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

public:
	void SetNetAddressable();

protected:
	virtual bool IsNameStableForNetworking() const override;
};
