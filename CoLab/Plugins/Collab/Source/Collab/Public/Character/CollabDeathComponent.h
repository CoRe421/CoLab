// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkComponent.h"
#include "CollabDeathComponent.generated.h"


class UCollabAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCollabDeath_DeathEvent, AActor*, OwningActor);

/**
 * ECollabDeathState
 *
 *	Defines current state of death.
 */
UENUM(BlueprintType)
enum class ECollabDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COLLAB_API UCollabDeathComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

protected:

	// Ability system used by this component.
	UPROPERTY()
	TWeakObjectPtr<UCollabAbilitySystemComponent> AbilitySystemComponent;

	// Replicated state used to handle dying.
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	ECollabDeathState DeathState;

public:
	// Delegate fired when the death sequence has started.
	UPROPERTY(BlueprintAssignable)
	FCollabDeath_DeathEvent OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	UPROPERTY(BlueprintAssignable)
	FCollabDeath_DeathEvent OnDeathFinished;

public:
	// Sets default values for this component's properties
	UCollabDeathComponent(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Returns the death component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Collab|Death")
	static UCollabDeathComponent* FindDeathComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UCollabDeathComponent>() : nullptr); }

	// Initialize the component using an ability system component.
	UFUNCTION(BlueprintCallable, Category = "Collab|Death")
	void InitializeWithAbilitySystem(UCollabAbilitySystemComponent* InASC);

	// Uninitialize the component, clearing any references to the ability system.
	UFUNCTION(BlueprintCallable, Category = "Collab|Death")
	void UninitializeFromAbilitySystem();

	UFUNCTION(BlueprintCallable, Category = "Collab|Death")
	ECollabDeathState GetDeathState() const { return DeathState; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Collab|Death", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return (DeathState > ECollabDeathState::NotDead); }

	// Begins the death sequence for the owner.
	virtual void StartDeath();

	// Ends the death sequence for the owner.
	virtual void FinishDeath();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnRegister() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UFUNCTION()
	virtual void OnRep_DeathState(ECollabDeathState OldDeathState);

	void ClearGameplayTags();
};
