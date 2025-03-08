// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollabCharacterBase.h"
#include "CollabPlayerCharacter.generated.h"

class UCollabPawnExtensionComponent;
class UCollabHeroComponent;

UCLASS()
class COLLAB_API ACollabPlayerCharacter : public ACollabCharacterBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<UCollabHeroComponent> HeroComponent;

public:
	// Sets default values for this character's properties
	ACollabPlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;

	virtual void OnAbilitySystemInitialized(UCollabAbilitySystemComponent* CollabASC) override;

	UFUNCTION(BlueprintNativeEvent)
	void SetupAbilitySystemComponentBindings();
	virtual void SetupAbilitySystemComponentBindings_Implementation();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
// private:
// 	void InitAbilitySystemComponent();
};
