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
	
	const UWorld* CurrentWorld = GetWorld();
	if (!ensureAlways(IsValid(CurrentWorld)))
	{
		return;
	}

	const ENetMode WorldNetMode = CurrentWorld->GetNetMode();
	if (WorldNetMode < NM_Client)
	{
		// For some reason these still fire on clients, but keeping the if statement for intent - CR
		FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &ThisClass::OnWorldActorsInitialized);
		FWorldDelegates::OnWorldBeginTearDown.AddUObject(this, &ThisClass::OnWorldBeginTeardown);
	}
}

void UCollabRuntimeConfigSubsystem::Deinitialize()
{
	Super::Deinitialize();

	int32 NumRemoved = 0;
	NumRemoved = FWorldDelegates::OnWorldInitializedActors.RemoveAll(this);
	UE_LOG(LogCollab, Log, TEXT("World Initialized delegates removed: %i"), NumRemoved);
	NumRemoved = FWorldDelegates::OnWorldBeginTearDown.RemoveAll(this);
	UE_LOG(LogCollab, Log, TEXT("World teardown delegates removed: %i"), NumRemoved);
}

void UCollabRuntimeConfigSubsystem::OnWorldActorsInitialized(const FActorsInitializedParams& Params)
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

	
	const ENetMode WorldNetMode = CurrentWorld->GetNetMode();
	if (WorldNetMode >= NM_Client)
	{
		return;
	}
	
	ensureAlways(TrySpawnConfigManager(CurrentWorld));
}

void UCollabRuntimeConfigSubsystem::OnWorldBeginTeardown(UWorld* World)
{
	if (!CachedConfigManager.IsValid())
	{
		return;
	}

	if (!IsValid(World) || World->GetNetMode() >= NM_Client)
	{
		return;
	}
	
	const ENetMode WorldNetMode = World->GetNetMode();
	if (WorldNetMode >= NM_Client)
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

	TArray<TTuple<FString, const FProperty*>> MetaProperties;
	TArray<const FProperty*> ConfigProperties;
	for (TFieldIterator<FProperty> PropIt(ConfigDataClass); PropIt; ++PropIt)
	{
		const FProperty* Property = *PropIt;

		if (!ensureAlways(Property))
		{
			continue;
		}

		const EPropertyFlags PropertyFlags = Property->GetPropertyFlags();
		// If not instance editable, don't allow modification - CR
		if (PropertyFlags & CPF_DisableEditOnInstance)
		{
			const FString PropertyName = Property->GetName();

			// Meta properties name's need to end with the MetaSpecifier, and not instance editable - CR
			if (PropertyName.EndsWith(UCollabConfigData::MetaSpecifier))
			{
				MetaProperties.Emplace(PropertyName, Property);
			}
		}
		else
		{
			ConfigProperties.Emplace(Property);
		}
	}

	for (const FProperty* ConfigProperty : ConfigProperties)
	{
		FCollabModifiablePropertyData NewPropertyData;
		if (!TryGetPropertyDataFromProperty(ConfigProperty, ConfigData, NewPropertyData))
		{
			continue;
		}
		
		const FString PropertyNameString = NewPropertyData.PropertyName.ToString();
		for (const TTuple<FString, const FProperty*>& MetaPropertyPair : MetaProperties)
		{
			const FString MetaPropertyName = MetaPropertyPair.Key;
			const FProperty* MetaProperty = MetaPropertyPair.Value;
			if (!ensureAlways(MetaProperty) || !MetaPropertyName.StartsWith(PropertyNameString))
			{
				continue;
			}

			const FString MetaNameMeta = MetaPropertyName.RightChop(PropertyNameString.Len());
			const FString MetaName = MetaNameMeta.LeftChop(UCollabConfigData::MetaSpecifier.Len());
			const void* MetaValue = MetaProperty->ContainerPtrToValuePtr<void>(ConfigData);
			const uint64 MetaCastFlags = MetaProperty->GetCastFlags();
			const FString MetaStringValue = SerializeConfigPropertyValue_Internal(MetaCastFlags, MetaValue);
			NewPropertyData.MetaData.Emplace(MetaName, MetaStringValue);
		}

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

bool UCollabRuntimeConfigSubsystem::TryGetDefaultDataForProperty(UCollabConfigData* ConfigData, const FName& PropertyName,
	FCollabModifiablePropertyData& DefaultPropertyData)
{
	if (!IsValid(ConfigData))
	{
		return false;
	}

	const UClass* ConfigDataClass = ConfigData->GetClass();
	if (!IsValid(ConfigDataClass))
	{
		return false;
	}

	const UCollabConfigData* DefaultConfigData = ConfigDataClass->GetDefaultObject<UCollabConfigData>();
	if (!IsValid(DefaultConfigData))
	{
		return false;
	}
	
	for (TFieldIterator<FProperty> PropIt(ConfigDataClass); PropIt; ++PropIt)
	{
		const FProperty* DefaultProperty = *PropIt;

		if (!ensureAlways(DefaultProperty))
		{
			continue;
		}

		const FName DefaultPropertyName = DefaultProperty->GetFName();
		if (DefaultPropertyName != PropertyName)
		{
			continue;
		}

		if (!TryGetPropertyDataFromProperty(DefaultProperty, DefaultConfigData, DefaultPropertyData))
		{
			continue;
		}
		
		return true;
	}

	return false;
}

bool UCollabRuntimeConfigSubsystem::TryGetDefaultDataForPropertyFromClass(UObject* WorldContextObject,
	const TSoftClassPtr<UCollabConfigData> ConfigDataClass, const FName& PropertyName,
	FCollabModifiablePropertyData& DefaultPropertyData)
{
	UCollabConfigData* ConfigData = GetConfigData(WorldContextObject, ConfigDataClass.LoadSynchronous());
	if (!IsValid(ConfigData))
	{
		return false;
	}

	return TryGetDefaultDataForProperty(ConfigData, PropertyName, DefaultPropertyData);
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
				float FloatValue = DeserializeConfigFloat(NewPropertyData.SerializedData);
				double DoubleValue = DeserializeConfigFloat(NewPropertyData.SerializedData);

				// const bool bIsNativeProperty = FoundProperty->IsNative();
				if (FoundProperty->IsA(FDoubleProperty::StaticClass()))
				{
					FoundProperty->SetValue_InContainer(ConfigData, &DoubleValue);
				}
				else
				{
					FoundProperty->SetValue_InContainer(ConfigData, &FloatValue);
				}

				FoundProperty->GetValue_InContainer(ConfigData, &FloatValue);
				FoundProperty->GetValue_InContainer(ConfigData, &DoubleValue);

				const double Dummy = FloatValue + DoubleValue;
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

	// CachedConfigManager->ForceNetUpdate();
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
	const uint64 CastFlags = Property->GetCastFlags();
	if (CastFlags & (CASTCLASS_FDoubleProperty | CASTCLASS_FFloatProperty))
	{
		PropertyType = ECollabModifiablePropertyType::Float;
	}
	else if (CastFlags & CASTCLASS_FIntProperty)
	{
		PropertyType = ECollabModifiablePropertyType::Int32;
	}
	else if (CastFlags & CASTCLASS_FBoolProperty)
	{
		PropertyType = ECollabModifiablePropertyType::Bool;
	} 
	else
	{
		// ensure(false);
		UE_LOG(LogCollab, Warning, TEXT("Cannot serialize property type"));
	}
		
	// increment instruction pointer unless it's null.
	// this is required to mark we finished traversing parameters through stack.
	P_FINISH
	
	P_NATIVE_BEGIN; // this macro lets profiler/insights know we're now evaluating a native logic, so it wont be displayed as Blueprint time.
	if (PropertyType.IsSet())
	{
		Result = SerializeConfigPropertyValue_Internal(CastFlags, ValuePtr);
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

FString UCollabRuntimeConfigSubsystem::SerializeConfigPropertyValue_Internal(const uint64& PropertyCastFlags,
	const void* Value)
{
	if (!ensureAlways(Value))
	{
		return FString();
	}
	
	FString Result;
	if (PropertyCastFlags & CASTCLASS_FDoubleProperty)
	{
		const double* DoubleValue = reinterpret_cast<const double*>(Value);
		check(DoubleValue);
		Result = FString::SanitizeFloat(*DoubleValue);
	}
	else if (PropertyCastFlags & CASTCLASS_FFloatProperty)
	{
		const float* FloatValue = reinterpret_cast<const float*>(Value);
		check(FloatValue);
		Result = FString::SanitizeFloat(*FloatValue);
	}
	else if (PropertyCastFlags & CASTCLASS_FIntProperty)
	{
		const int32* IntValue = reinterpret_cast<const int32*>(Value);
		check(IntValue);
		Result = FString::SanitizeFloat(*IntValue);
	}
	else if (PropertyCastFlags & CASTCLASS_FBoolProperty)
	{
		const bool* BoolValue = reinterpret_cast<const bool*>(Value);
		check(BoolValue);
		Result = UKismetStringLibrary::Conv_BoolToString(*BoolValue);
	} 
	else
	{
		ensure(false);
		UE_LOG(LogCollab, Warning, TEXT("Cannot serialize property type"));
	}
	// switch (Type)
	// {
	// case (ECollabModifiablePropertyType::Float):
	// 	{
	// 		constexpr int BIG_EXPONENT = 8;
	// 		constexpr float ZERO_TOLERANCE = 0.0001f;
	// 		constexpr float EXCESSIVE_PRECISION_DIGITS = 3.f;
	// 		const float ExcessiveCoefficient = FMath::Pow(10, EXCESSIVE_PRECISION_DIGITS);
	// 		
	// 		const float* FloatValue = reinterpret_cast<const float*>(Value);
	// 		check(FloatValue);
	// 		const bool bFloatIsZero = FMath::IsNearlyZero(*FloatValue, ZERO_TOLERANCE);
	// 		const bool bFloatIsBig = FMath::LogX(10, *FloatValue) > BIG_EXPONENT;
	// 		const double BigTruncFloat = FMath::TruncToInt(static_cast<double>(*FloatValue * ExcessiveCoefficient));
	// 		const double BigFloat = *FloatValue * ExcessiveCoefficient;
	// 		const bool bFloatHasExcessPrecision = !FMath::IsNearlyEqual(BigTruncFloat, BigFloat);
	// 		
	// 		const double* DoubleValue = reinterpret_cast<const double*>(Value);
	// 		check(DoubleValue);
	// 		const bool bDoubleIsZero = FMath::IsNearlyZero(*DoubleValue, ZERO_TOLERANCE);
	// 		const bool bDoubleIsBig = FMath::LogX(10, *DoubleValue) > BIG_EXPONENT;
	// 		const double BigTruncDouble = FMath::TruncToInt(static_cast<double>(*DoubleValue * ExcessiveCoefficient));
	// 		const double BigDouble = *DoubleValue * ExcessiveCoefficient;
	// 		const bool bDoubleHasExcessPrecision = !FMath::IsNearlyEqual(BigTruncDouble, BigDouble);
	//
	// 		// const int Coefficient = FMath::Pow(10.f, NUM_DIGITS_EXCESSIVE);
	// 		// double TempDouble = *DoubleValue * Coefficient;
	// 		// const double DoubleRemainder = FMath::Modf(TempDouble, &TempDouble);
	// 		// const bool bHasExcessiveValues = !FMath::IsNearlyZero(DoubleRemainder) && !FMath::IsNearlyEqual(DoubleRemainder, 1);
	// 		if (!bDoubleIsZero && !bDoubleIsBig && (!bDoubleHasExcessPrecision || bFloatHasExcessPrecision))
	// 		{
	// 			Result = FString::SanitizeFloat(*DoubleValue);
	// 		}
	// 		else if (ensureAlways((!bFloatIsZero || !bFloatHasExcessPrecision) && !bFloatIsBig))
	// 		{
	// 			Result = FString::SanitizeFloat(*FloatValue);
	// 		}
	// 		else
	// 		{
	// 			return FString::SanitizeFloat(0.0f);
	// 		}
	// 	} break;
	// case (ECollabModifiablePropertyType::Int32):
	// 	{
	// 		const int32* IntValue = reinterpret_cast<const int32*>(Value);
	// 		if (IntValue)
	// 		{
	// 			Result = FString::FromInt(*IntValue);
	// 		}
	// 	} break;
	// case (ECollabModifiablePropertyType::Bool):
	// 	{
	// 		const bool* BoolValue = reinterpret_cast<const bool*>(Value);
	// 		if (BoolValue)
	// 		{
	// 			Result = UKismetStringLibrary::Conv_BoolToString(*BoolValue);
	// 		}
	// 	} break;
	// default:
	// 	{
	// 		ensure(false);
	// 		UE_LOG(LogCollab, Warning, TEXT("Cannot deserialize property type"));
	// 	} break;
	// }

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

	UWorld* World = WorldContext->GetWorld();
	if (!IsValid(World))
	{
		return false;
	}

	const ENetMode WorldNetMode = World->GetNetMode();

	AActor* FoundActor = UGameplayStatics::GetActorOfClass(WorldContext, ACollabConfigManager::StaticClass());
	ACollabConfigManager* FoundConfigManager = Cast<ACollabConfigManager>(FoundActor);
	if (!IsValid(FoundConfigManager))
	{
		UE_LOG(LogCollab, Error, TEXT("Config Manager not found! Was game instance initialized on server?"));
		return false;
	}
	
	CachedConfigManager = FoundConfigManager;
	return true;
}

bool UCollabRuntimeConfigSubsystem::TrySpawnConfigManager(const UObject* WorldContext)
{
	if (CachedConfigManager.IsValid())
	{
		return true;
	}

	if (!IsValid(WorldContext))
	{
		return false;
	}

	UWorld* World = WorldContext->GetWorld();
	if (!IsValid(World))
	{
		return false;
	}
	
	const ENetMode WorldNetMode = World->GetNetMode();
	if (!ensureAlways(WorldNetMode < NM_Client))
	{
		UE_LOG(LogCollab, Error, TEXT("Cannot spawn config manager if not on the server!"));
		return false;
	}

	AActor* FoundActor = UGameplayStatics::GetActorOfClass(WorldContext, ACollabConfigManager::StaticClass());
	ACollabConfigManager* FoundConfigManager = Cast<ACollabConfigManager>(FoundActor);
	if (IsValid(FoundConfigManager))
	{
		return true;
	}

	FoundConfigManager = World->SpawnActor<ACollabConfigManager>();
	if (!ensureAlways(IsValid(FoundConfigManager)))
	{
		return false;
	}
	
	if (!TempConfigData.IsEmpty())
	{
		FoundConfigManager->InitializeConfigData(TempConfigData);
		TempConfigData.Reset();
	}

	return true;
}

bool UCollabRuntimeConfigSubsystem::TryGetPropertyDataFromProperty(const FProperty* Property, const UObject* Container, FCollabModifiablePropertyData& PropertyData) const
{
	if (!ensureAlways(Property))
	{
		return false;
	}
		
	PropertyData.PropertyName = Property->GetFName();
	const TMap<FName, FString> PropertyMetaData = *Property->GetMetaDataMap();
	PropertyData.MetaData = PropertyMetaData;

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
		// ensure(false);
		UE_LOG(LogCollab, Warning, TEXT("Cannot de-serialize property type"));
		return false;
	}

	if (!FoundType.IsSet())
	{
		return false;
	}

	PropertyData.Type = FoundType.GetValue();
	const void* Value = Property->ContainerPtrToValuePtr<void>(Container);
	PropertyData.SerializedData = SerializeConfigPropertyValue_Internal(CastFlags, Value);

	return true;
}
