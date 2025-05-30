// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CollabFunctionLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(bool, FDynamicCallback_SortDelegate, const UObject*, A, const UObject*, B);
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FDynamicCallback_FilterDelegate, const UObject*, Object);

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

	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(DefaultToSelf="Actor"))
	static bool IsActorOnClient(const AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(DefaultToSelf="Actor"))
	static bool IsActorOnServer(const AActor* Actor);

	UFUNCTION(BlueprintCallable)
	static void SortArray(UPARAM(ref) TArray<UObject*>& Array, const FDynamicCallback_SortDelegate& SortDelegate);

	UFUNCTION(BlueprintCallable)
	static UPARAM(DisplayName="Filtered Array") TArray<UObject*> FilterArray(const TArray<UObject*>& Array, const FDynamicCallback_FilterDelegate& FilterDelegate);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(WorldContext="WorldContext"))
	static UPARAM(DisplayName="bIsPIE") bool IsPlayingInEditor(UObject* WorldContext);
};
