// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilitySystemComponent.h"
#include "CollabAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDynamicEvent_ReceiveDamage, UCollabAbilitySystemComponent*, SourceASC, float, UnmitigatedDamage, float, MitigatedDamage);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COLLAB_API UCollabAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCollabAbilitySystemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void AbilityInputPressed(const FGameplayTag& InputTag);
	void AbilityInputReleased(const FGameplayTag& InputTag);

	UFUNCTION(BlueprintCallable)
	FGameplayAbilitySpecHandle GiveCharacterAbility(FGameplayAbilitySpec& AbilitySpec);
};
