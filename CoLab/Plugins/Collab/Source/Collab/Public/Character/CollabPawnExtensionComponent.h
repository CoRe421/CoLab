// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/PawnComponent.h"
#include "CollabPawnExtensionComponent.generated.h"


class UCollabAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COLLAB_API UCollabPawnExtensionComponent : public UPawnComponent
{
	GENERATED_BODY()

protected:
	TWeakObjectPtr<UCollabAbilitySystemComponent> AbilitySystemComponent;

public:
	// Sets default values for this component's properties
	UCollabPawnExtensionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction) override;

	/** Returns the pawn extension component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Collab|Pawn")
	static UCollabPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UCollabPawnExtensionComponent>() : nullptr); }
	
	UFUNCTION(BlueprintPure)
	UCollabAbilitySystemComponent* GetCollabAbilitySystemComponent() const { return AbilitySystemComponent.Get(); }

	/** Should be called by the owning pawn to become the avatar of the ability system. */
	void InitializeAbilitySystem(UCollabAbilitySystemComponent* InASC, AActor* InOwnerActor);

	/** Should be called by the owning pawn to remove itself as the avatar of the ability system. */
	void UninitializeAbilitySystem();
};
