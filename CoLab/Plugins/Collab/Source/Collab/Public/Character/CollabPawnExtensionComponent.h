// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// #include "CoreMinimal.h"
// #include "Components/ActorComponent.h"
// #include "Components/PawnComponent.h"
// #include "CollabPawnExtensionComponent.generated.h"


// class UCollabPawnData;
// class UCollabAbilitySystemComponent;
//
// UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
// class COLLAB_API UCollabPawnExtensionComponent : public UPawnComponent
// {
// 	GENERATED_BODY()
//
// protected:
// 	TWeakObjectPtr<UCollabAbilitySystemComponent> AbilitySystemComponent;
//
// 	/** Pawn data used to create the pawn. Specified from a spawn function or on a placed instance. */
// 	// Given from the PlayerState - CR
// 	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "Collab|Pawn")
// 	TObjectPtr<const UCollabPawnData> PawnData;
//
// public:
// 	// Sets default values for this component's properties
// 	UCollabPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);
//
// protected:
// 	// Called when the game starts
// 	virtual void BeginPlay() override;
//
// 	UFUNCTION()
// 	void OnRep_PawnData();
//
// public:
// 	// Called every frame
// 	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
// 	FActorComponentTickFunction* ThisTickFunction) override;
//
// 	/** Returns the pawn extension component if one exists on the specified actor. */
// 	UFUNCTION(BlueprintPure, Category = "Collab|Pawn")
// 	static UCollabPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UCollabPawnExtensionComponent>() : nullptr); }
//
// 	/** Gets the pawn data, which is used to specify pawn properties in data */
// 	template <class T>
// 	const T* GetPawnData() const { return Cast<T>(PawnData); }
//
// 	/** Sets the current pawn data */
// 	void SetPawnData(const UCollabPawnData* InPawnData);
// 	
// 	UFUNCTION(BlueprintPure)
// 	UCollabAbilitySystemComponent* GetCollabAbilitySystemComponent() const { return AbilitySystemComponent.Get(); }
//
// 	/** Should be called by the owning pawn to become the avatar of the ability system. */
// 	void InitializeAbilitySystem(UCollabAbilitySystemComponent* InASC, AActor* InOwnerActor);
//
// 	/** Should be called by the owning pawn to remove itself as the avatar of the ability system. */
// 	void UninitializeAbilitySystem();
// };
