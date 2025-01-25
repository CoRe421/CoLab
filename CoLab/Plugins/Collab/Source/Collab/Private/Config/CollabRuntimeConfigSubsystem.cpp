// Fill out your copyright notice in the Description page of Project Settings.


#include "Config/CollabRuntimeConfigSubsystem.h"

#include "CollabLog.h"
#include "Config/CollabConfigData.h"
#include "Kismet/KismetStringLibrary.h"
#include "UObject/UnrealTypePrivate.h"

UCollabConfigData* UCollabRuntimeConfigSubsystem::GetConfigData(const TSoftClassPtr<UCollabConfigData> ConfigDataClass)
{
	const TObjectPtr<UCollabConfigData>* FoundConfig = ConstructedConfigs.Find(ConfigDataClass);
	if (FoundConfig && IsValid(*FoundConfig))
	{
		return *FoundConfig;
	}

	const UClass* LoadedConfigClass = ConfigDataClass.LoadSynchronous();
	if (!IsValid(LoadedConfigClass) || !LoadedConfigClass->IsChildOf<UCollabConfigData>())
	{
		return nullptr;
	}

	UCollabConfigData* NewConfig = NewObject<UCollabConfigData>(this, LoadedConfigClass);
	if (!ensureAlways(IsValid(NewConfig)))
	{
		return nullptr;
	}

	ConstructedConfigs.Emplace(ConfigDataClass, NewConfig);
	return NewConfig;
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

void UCollabRuntimeConfigSubsystem::GetConfigPropertyDataFromClass(
	const TSoftClassPtr<UCollabConfigData> ConfigDataClass, TArray<struct FCollabModifiablePropertyData>& PropertyData)
{
	UCollabConfigData* ConfigData = GetConfigData(ConfigDataClass);
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
				float FloatValue = DeserializeConfigFloat(NewPropertyData.SerializedData);
				FoundProperty->SetValue_InContainer(ConfigData, &FloatValue);
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

void UCollabRuntimeConfigSubsystem::SetConfigPropertyDataFromClass(const TSoftClassPtr<UCollabConfigData> ConfigDataClass,
	const TArray<FCollabModifiablePropertyData>& PropertyData)
{
	UCollabConfigData* ConfigData = GetConfigData(ConfigDataClass);
	if (!IsValid(ConfigData))
	{
		return;
	}

	SetConfigPropertyData(ConfigData, PropertyData);
}

float UCollabRuntimeConfigSubsystem::DeserializeConfigFloat(const FString& Value)
{
	float FloatValue;
	LexFromString(FloatValue, *Value);
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
		
	// we know our return type is float, and RESULT_PARAM is a void* that points to the return parameter of this function in the "parms memory"
	// it's size is same as float too, because thats how BPVM works, so we want to re-interpret it as float and set our result variable to it.
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
			const double* FloatValue = reinterpret_cast<const double*>(Value); -- Need to make this work with floats too
			if (FloatValue)
			{
				Result = FString::SanitizeFloat(*FloatValue);
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
