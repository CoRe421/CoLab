// Fill out your copyright notice in the Description page of Project Settings.


#include "Config/CollabRuntimeConfigSubsystem.h"

#include "CollabLog.h"
#include "Config/CollabConfigData.h"
#include "Config/CollabConfigManager.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Net/UnrealNetwork.h"
#include "UObject/UnrealTypePrivate.h"

void UCollabRuntimeConfigSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FWorldDelegates::OnWorldInitializedActors.AddLambda([&](const FActorsInitializedParams& Params)
	{
		const UWorld* CurrentWorld = Params.World;
		if (!IsValid(CurrentWorld) || CachedConfigManager.IsValid())
		{
			return;
		}

		if (CurrentWorld->WorldType != EWorldType::Game && CurrentWorld->WorldType != EWorldType::PIE)
		{
			return;
		}

		// No longer spawn config managers directly, causes issues with replication - CR
		// This means that configs aren't preserved between levels, and only work when a config manager is placed in the level - CR
		// TODO: Can explore duplicating config manager configs on level unloading, and initialize next config manager with it - CR

		TryCacheConfigManager(CurrentWorld);
	});
	
	const UWorld* CurrentWorld = GetWorld();
	if (ensureAlways(IsValid(CurrentWorld)))
	{
		if (CurrentWorld->GetNetMode() < NM_Client)
		{
			FWorldDelegates::OnWorldBeginTearDown.AddLambda([&](const UWorld* World)
			{
				if (!CachedConfigManager.IsValid())
				{
					return;
				}

				if (!IsValid(World) || World->GetNetMode() >= NM_Client)
				{
					return;
				}
				
				const TArray<TObjectPtr<UCollabConfigData>> ConfigManagerConfigs = CachedConfigManager->
					ConstructedConfigs;

				if (!ensureAlways(TempConfigData.IsEmpty()))
				{
					TempConfigData.Reset(ConfigManagerConfigs.Num());
				}
				else
				{
					TempConfigData.SetNumZeroed(ConfigManagerConfigs.Num());
				}
				
				// We're commandeering the configs from the manager so we can store them in the next manager - CR
				for (const TObjectPtr<UCollabConfigData>& Config : ConfigManagerConfigs)
				{
					const FName CurrentName = Config->GetFName();
					Config->Rename(*CurrentName.ToString(), this);

					TempConfigData.Emplace(Config);
				}
			});
		}
	}
}

UCollabConfigData* UCollabRuntimeConfigSubsystem::GetConfigData(UObject* WorldContextObject, const TSubclassOf<UCollabConfigData> ConfigDataClass)
{
	ACollabConfigManager* ConfigManager = GetConfigManager(WorldContextObject);
	if (!IsValid(ConfigManager))
	{
		return nullptr;
	}

	UCollabConfigData* ConfigData = ConfigManager->FindOrCreateConfigData(ConfigDataClass);
	if (!IsValid(ConfigData))
	{
		return nullptr;
	}

	return ConfigData;
}

void UCollabRuntimeConfigSubsystem::GetConfigPropertyData(UCollabConfigData* ConfigData, TArray<struct FCollabModifiablePropertyData>& PropertyData)
{
	if (!IsValid(ConfigData))
	{
		return;
	}
	
	const UClass* ConfigDataClass = ConfigData->GetClass();
	if (!IsValid(ConfigDataClass))
	{
		return;
	}
	
	for (TFieldIterator<FProperty> PropIt(ConfigDataClass); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;

		if (!ensureAlways(Property))
		{
			continue;
		}
		
		FCollabModifiablePropertyData NewPropertyData;
		NewPropertyData.PropertyName = Property->NamePrivate;
		NewPropertyData.MetaData = *Property->GetMetaDataMap();

		TOptional<ECollabModifiablePropertyType> FoundType;
		const uint64 CastFlags = Property->GetCastFlags();
		if (CastFlags & (CASTCLASS_FDoubleProperty | CASTCLASS_FFloatProperty))
		{
			FoundType = ECollabModifiablePropertyType::Float;
		}
		else if (CastFlags & CASTCLASS_FIntProperty)
		{
			FoundType = ECollabModifiablePropertyType::Int32;
		}
		else if (CastFlags & CASTCLASS_FBoolProperty)
		{
			FoundType = ECollabModifiablePropertyType::Bool;
		} 
		else
		{
			ensure(false);
			UE_LOG(LogCollab, Warning, TEXT("Cannot de-serialize property type"));
			continue;
		}

		if (!FoundType.IsSet())
		{
			continue;
		}

		NewPropertyData.Type = FoundType.GetValue();
		const void* Value = Property->ContainerPtrToValuePtr<void>(ConfigData);
		NewPropertyData.SerializedData = SerializeConfigPropertyValue_Internal(NewPropertyData.Type, Value);

		PropertyData.Emplace(NewPropertyData);
	}
}

void UCollabRuntimeConfigSubsystem::GetConfigPropertyDataFromClass(UObject* WorldContextObject, 
	const TSoftClassPtr<UCollabConfigData> ConfigDataClass, TArray<struct FCollabModifiablePropertyData>& PropertyData)
{
	UCollabConfigData* ConfigData = GetConfigData(WorldContextObject, ConfigDataClass.LoadSynchronous());
	if (!IsValid(ConfigData))
	{
		return;
	}

	GetConfigPropertyData(ConfigData, PropertyData);
}

void UCollabRuntimeConfigSubsystem::SetConfigPropertyData(UCollabConfigData* ConfigData,
	const TArray<FCollabModifiablePropertyData>& PropertyData)
{
	if (!IsValid(ConfigData))
	{
		return;
	}
	
	const UClass* ConfigDataClass = ConfigData->GetClass();
	if (!IsValid(ConfigDataClass))
	{
		return;
	}

	for (const FCollabModifiablePropertyData& NewPropertyData : PropertyData)
	{
		FProperty* FoundProperty = ConfigDataClass->FindPropertyByName(NewPropertyData.PropertyName);
		if (!ensure(FoundProperty))
		{
			continue;
		}

		switch (NewPropertyData.Type)
		{
		case ECollabModifiablePropertyType::Float:
			{
				// Needs to be stored as a double because apparently floats in BPs are actually doubles - CR
				double DoubleValue = DeserializeConfigFloat(NewPropertyData.SerializedData);
				FoundProperty->SetValue_InContainer(ConfigData, &DoubleValue);
			} break;
		case ECollabModifiablePropertyType::Int32:
			{
				int32 IntValue = DeserializeConfigInt32(NewPropertyData.SerializedData);
				FoundProperty->SetValue_InContainer(ConfigData, &IntValue);
			} break;
		case ECollabModifiablePropertyType::Bool:
			{
				bool BoolValue = DeserializeConfigBool(NewPropertyData.SerializedData);
				FoundProperty->SetValue_InContainer(ConfigData, &BoolValue);
			} break;
		default:
			{
				ensure(false);
				UE_LOG(LogCollab, Warning, TEXT("Cannot serialize property type"));
				continue;
			} break;
		}
	}
}

void UCollabRuntimeConfigSubsystem::SetConfigPropertyDataFromClass(UObject* WorldContextObject,
	const TSoftClassPtr<UCollabConfigData> ConfigDataClass, const TArray<FCollabModifiablePropertyData>& PropertyData)
{
	UCollabConfigData* ConfigData = GetConfigData(WorldContextObject, ConfigDataClass.LoadSynchronous());
	if (!IsValid(ConfigData))
	{
		return;
	}

	SetConfigPropertyData(ConfigData, PropertyData);
}

float UCollabRuntimeConfigSubsystem::DeserializeConfigFloat(const FString& Value)
{
	double DoubleValue;
	float FloatValue;
	LexFromString(DoubleValue, *Value);
	if (FMath::IsNearlyZero(DoubleValue))
	{
		LexFromString(FloatValue, *Value);
	}
	else
	{
		FloatValue = DoubleValue;
	}
	return FloatValue;
}

int32 UCollabRuntimeConfigSubsystem::DeserializeConfigInt32(const FString& Value)
{
	int32 IntValue;
	LexFromString(IntValue, *Value);
	return IntValue;
}

bool UCollabRuntimeConfigSubsystem::DeserializeConfigBool(const FString& Value)
{
	bool BoolValue;
	LexFromString(BoolValue, *Value);
	return BoolValue;
}

FString UCollabRuntimeConfigSubsystem::SerializeConfigPropertyValue(const int32& Value)
{
	check(0);
	return FString();
}

DEFINE_FUNCTION(UCollabRuntimeConfigSubsystem::execSerializeConfigPropertyValue)
{
	//Move stack parameter index to the first parameter
	Stack.MostRecentProperty = nullptr;
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.StepCompiledIn<FProperty>(nullptr);

	const FProperty* Property = Stack.MostRecentProperty;
		
	FString Result;
	const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Stack.MostRecentPropertyContainer);
	//Check type
	TOptional<ECollabModifiablePropertyType> PropertyType;
	if (Property->IsA(FDoubleProperty::StaticClass()) || Property->IsA(FFloatProperty::StaticClass()))
	{
		PropertyType = ECollabModifiablePropertyType::Float;
	}
	else if (Property->IsA(FIntProperty::StaticClass()))
	{
		PropertyType = ECollabModifiablePropertyType::Int32;
	}
	else if (Property->IsA(FBoolProperty::StaticClass()))
	{
		PropertyType = ECollabModifiablePropertyType::Bool;
	}
		
	// increment instruction pointer unless it's null.
	// this is required to mark we finished traversing parameters through stack.
	P_FINISH
	
	P_NATIVE_BEGIN; // this macro lets profiler/insights know we're now evaluating a native logic, so it wont be displayed as Blueprint time.
	if (PropertyType.IsSet())
	{
		Result = SerializeConfigPropertyValue_Internal(PropertyType.GetValue(), ValuePtr);
	}
	else
	{
		ensure(false);
		UE_LOG(LogCollab, Warning, TEXT("Cannot serialize property type"));
	}
	P_NATIVE_END; // let profiler know native logic has ended.
		
	// we know our return type is FString, and RESULT_PARAM is a void* that points to the return parameter of this function in the "parms memory"
	// it's size is same as FString too, because thats how BPVM works, so we want to re-interpret it as FString and set our result variable to it.
	*reinterpret_cast<FString*>(RESULT_PARAM) = Result;
}

FString UCollabRuntimeConfigSubsystem::SerializeConfigPropertyValue_Internal(const ECollabModifiablePropertyType& Type,
	const void* Value)
{
	FString Result;
	switch (Type)
	{
	case (ECollabModifiablePropertyType::Float):
		{
			const double* DoubleValue = reinterpret_cast<const double*>(Value);
			const float* FloatValue = reinterpret_cast<const float*>(Value);
			if (DoubleValue && !FMath::IsNearlyZero(*DoubleValue))
			{
				Result = FString::SanitizeFloat(*DoubleValue);
			}
			else if (FloatValue)
			{
				Result = FString::SanitizeFloat(*FloatValue);
			}
			else
			{
				ensure(false);
				UE_LOG(LogCollab, Warning, TEXT("Cannot serialize float"));
			}
		} break;
	case (ECollabModifiablePropertyType::Int32):
		{
			const int32* IntValue = reinterpret_cast<const int32*>(Value);
			if (IntValue)
			{
				Result = FString::FromInt(*IntValue);
			}
		} break;
	case (ECollabModifiablePropertyType::Bool):
		{
			const bool* BoolValue = reinterpret_cast<const bool*>(Value);
			if (BoolValue)
			{
				Result = UKismetStringLibrary::Conv_BoolToString(*BoolValue);
			}
		} break;
	default:
		{
			ensure(false);
			UE_LOG(LogCollab, Warning, TEXT("Cannot deserialize property type"));
		} break;
	}

	return Result;
}

ACollabConfigManager* UCollabRuntimeConfigSubsystem::GetConfigManager(const UObject* WorldContext)
{
	if (!CachedConfigManager.IsValid())
	{
		if (!TryCacheConfigManager(WorldContext))
		{
			return nullptr;
		}
	}
	
	return CachedConfigManager.Get();
}

bool UCollabRuntimeConfigSubsystem::TryCacheConfigManager(const UObject* WorldContext)
{
	if (CachedConfigManager.IsValid())
	{
		return true;
	}

	if (!IsValid(WorldContext))
	{
		return false;
	}

	AActor* FoundActor = UGameplayStatics::GetActorOfClass(WorldContext, ACollabConfigManager::StaticClass());
	ACollabConfigManager* FoundConfigManager = Cast<ACollabConfigManager>(FoundActor);
	if (!ensureAlways(IsValid(FoundConfigManager)))
	{
		UE_LOG(LogCollab, Error, TEXT("Config Manager not found! Was game instance initialized on server?"))
		return false;
	}

	if (FoundConfigManager->GetNetMode())
	{
		FoundConfigManager->InitializeConfigData(TempConfigData);
		TempConfigData.Empty();
	}
	
	CachedConfigManager = FoundConfigManager;
	return true;
}
