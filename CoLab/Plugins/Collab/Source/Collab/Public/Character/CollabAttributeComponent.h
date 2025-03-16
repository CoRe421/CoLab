// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkComponent.h"
#include "GameplayAbilitySystem/Attributes/CollabAttributeSet.h"
#include "CollabAttributeComponent.generated.h"

// DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDynamicCallback_OnAttributeChanged, const FGameplayAttribute&, Attribute, const float, OldValue, const float, NewValue);
//
// class UCollabAbilitySystemComponent;
//
// USTRUCT()
// struct FObjectCallbackContainer
// {
// 	GENERATED_BODY()
// 	
// public:
// 	UPROPERTY()
// 	TMap<TWeakObjectPtr<const UObject>, FDynamicCallback_OnAttributeChanged> ObjectCallbacks;
// };

// If replicating
// USTRUCT()
// struct FAttributeCallbackContainer
// {
// 	GENERATED_BODY()
//
// public:
// 	UPROPERTY()
// 	TSoftClassPtr<UCollabAttributeSet> AttributeSet;
// 	UPROPERTY()
// 	TMap<TWeakObjectPtr<const UObject>, FDynamicCallback_OnAttributeChanged> ObjectCallbacks;
// 	
// 	FAttributeCallbackContainer()
// 	{
// 	}
//
// 	FAttributeCallbackContainer(const TSoftClassPtr<UCollabAttributeSet>& AttributeSet) : AttributeSet(AttributeSet)
// 	{
// }

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COLLAB_API UCollabAttributeComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

	// Replaced by 'UAbilitySystemComponent::GetGameplayAttributeValueChangeDelegate()' and 'UAbilityAsync_WaitAttributeChanged' in BPs
// protected:
//
// 	// Ability system used by this component.
// 	UPROPERTY()
// 	TWeakObjectPtr<UCollabAbilitySystemComponent> AbilitySystemComponent;
//
// 	UPROPERTY()
// 	TMap<FGameplayAttribute, FObjectCallbackContainer> AttributeChangedCallbacks;
//
// 	// If replicating
// 	// UPROPERTY(Replicated)
// 	// TArray<FAttributeCallbackContainer> AttributeChangedCallbacks;
//
// public:
// 	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess), DisplayName="OnAttributeChanged")
// 	FDynamicEvent_OnAttributeChanged OnAttributeChanged_BP;
//
// public:
// 	// Sets default values for this component's properties
// 	UCollabAttributeComponent(const FObjectInitializer& ObjectInitializer);
//
// 	// If replicating
// 	// virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
//
// 	// Initialize the component using an ability system component.
// 	UFUNCTION(BlueprintCallable, Category = "Collab|Health")
// 	void InitializeWithAbilitySystem(UCollabAbilitySystemComponent* InASC);
//
// 	// Uninitialize the component, clearing any references to the ability system.
// 	UFUNCTION(BlueprintCallable, Category = "Collab|Health")
// 	void UninitializeFromAbilitySystem();
//
// 	// Returns the health component if one exists on the specified actor.
// 	UFUNCTION(BlueprintPure, Category = "Collab|Attributes")
// 	static UCollabAttributeComponent* FindAttributeComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UCollabAttributeComponent>() : nullptr); }
//
// protected:
// 	// Called when the game starts
// 	virtual void BeginPlay() override;
//
// 	virtual void OnRegister() override;
// 	virtual void OnUnregister() override;
//
// public:
// 	// Called every frame
// 	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
// 	                           FActorComponentTickFunction* ThisTickFunction) override;
//
// public:
// 	UFUNCTION(BlueprintCallable, meta=(HidePin="Owner", DefaultToSelf="Owner"))
// 	void RegisterAttributeChangedCallback(const UObject* Owner, const FGameplayAttribute Attribute, const FDynamicCallback_OnAttributeChanged& Callback);
// 	UFUNCTION(BlueprintCallable, meta=(HidePin="Owner", DefaultToSelf="Owner"))
// 	void ClearCallbackForAttribute(const UObject* Owner, const FGameplayAttribute Attribute);
//
// protected:
// 	UFUNCTION()
// 	void OnAttributeChanged(const FGameplayAttribute& Attribute, const float OldValue, const float NewValue);
//
// 	// If replicating
// 	// bool FindOrAddAttributeContainer(const TSoftClassPtr<UCollabAttributeSet>& AttributeSet, const UObject* Owner, FAttributeCallbackContainer& OutContainer);
};
