// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "CollabRuntimeConfigSubsystem.generated.h"

UENUM(BlueprintType)
enum class ECollabModifiablePropertyType : uint8
{
	Float,
	Int32,
	Bool
};

USTRUCT(BlueprintType)
struct FCollabModifiablePropertyData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PropertyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECollabModifiablePropertyType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SerializedData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FString> MetaData;
};

class UCollabConfigData;
/**
 * 
 */
UCLASS()
class COLLAB_API UCollabRuntimeConfigSubsystem final : public UEngineSubsystem
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TMap<TSoftClassPtr<UCollabConfigData>, TObjectPtr<UCollabConfigData>> ConstructedConfigs;

public:
	UFUNCTION(BlueprintCallable, Category="Collab|Config")
	UCollabConfigData* GetConfigData(const TSoftClassPtr<UCollabConfigData> ConfigDataClass);

	UFUNCTION(BlueprintCallable, Category="Collab|Config")
	void GetConfigPropertyData(UCollabConfigData* ConfigData, TArray<struct FCollabModifiablePropertyData>& PropertyData);
	UFUNCTION(BlueprintCallable, Category="Collab|Config")
	void GetConfigPropertyDataFromClass(const TSoftClassPtr<UCollabConfigData> ConfigDataClass, TArray<struct FCollabModifiablePropertyData>& PropertyData);

	UFUNCTION(BlueprintCallable, Category="Collab|Config")
	void SetConfigPropertyData(UCollabConfigData* ConfigData, const TArray<struct FCollabModifiablePropertyData>& PropertyData);
	UFUNCTION(BlueprintCallable, Category="Collab|Config")
	void SetConfigPropertyDataFromClass(const TSoftClassPtr<UCollabConfigData> ConfigDataClass, const TArray<struct FCollabModifiablePropertyData>& PropertyData);

	
	UFUNCTION(BlueprintPure, Category="Collab|Config")
	static float DeserializeConfigFloat(const FString& Value);
	UFUNCTION(BlueprintPure, Category="Collab|Config")
	static int32 DeserializeConfigInt32(const FString& Value);
	UFUNCTION(BlueprintPure, Category="Collab|Config")
	static bool DeserializeConfigBool(const FString& Value);
	
	UFUNCTION(BlueprintPure, Category="Collab|Config", CustomThunk, meta=(CustomStructureParam="Value"))
	static FString SerializeConfigPropertyValue(const int32& Value);
	DECLARE_FUNCTION(execSerializeConfigPropertyValue);

private:
	static FString SerializeConfigPropertyValue_Internal(const ECollabModifiablePropertyType& Type, const void* Value);
};
