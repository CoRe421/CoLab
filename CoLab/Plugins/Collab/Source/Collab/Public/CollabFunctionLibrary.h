// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CollabFunctionLibrary.generated.h"

class UAbilitySystemComponent;
class UCollabAttributeSet;
/**
 * 
 */
UCLASS()
class COLLAB_API UCollabFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType="AttributeSet"))
	static const UCollabAttributeSet* GetCollabAttributeSet(const UAbilitySystemComponent* AbilitySystemComponent, const TSubclassOf<UCollabAttributeSet> AttributeSet);

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType="Class", DefaultToSelf="Outer"))
	static UObject* ConstructObjectFromClassWithTemplate(UObject* Outer, const TSubclassOf<UObject> Class, UObject* Template);
};
