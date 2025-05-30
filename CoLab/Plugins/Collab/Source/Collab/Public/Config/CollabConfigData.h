// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CollabConfigData.generated.h"

// #ifdef ALLOW_METADATA_MACRO
// 	#define META_PROPERTY AddMetaProperty
// #endif

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicEvent_OnPropertyChanged, const FName, PropertyName);

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class COLLAB_API UCollabConfigData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable)
	FDynamicEvent_OnPropertyChanged OnConfigPropertyChanged;

public:
	// Properties whose names END with the meta specifier, START with the name of another property, and ARE NOT
	// instance editable will be added to that other property's metadata when config properties are accessed - CR
	static inline FString MetaSpecifier = "Meta";

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(META_PROPERTY))
	FText DisplayName;
// 	
// 	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
// 	float FloatTruncTest;
// 	
// 	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
// 	double DoubleTruncTest;

public:
	UCollabConfigData();

protected:
	virtual bool IsSupportedForNetworking() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
