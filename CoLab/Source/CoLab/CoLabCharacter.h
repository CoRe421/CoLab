// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/CollabCharacterBase.h"
#include "Character/CollabPlayerCharacter.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "CoLabCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ACoLabCharacter : public ACollabPlayerCharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** Pawn mesh: 3rd person view (body; seen by everyone) */
	// TODO: Change to skeletal mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh3P;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* BodyMesh;
	
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
	
	// /** Jump Input Action */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	// UInputAction* JumpAction;
	//
	// /** Move Input Action */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	// UInputAction* MoveAction;
	
public:
	ACoLabCharacter();

protected:
	virtual void BeginPlay();

// public:
// 		
// 	/** Look Input Action */
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
// 	class UInputAction* LookAction;
// 	
// 	/** Look Input Action */
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
// 	class UInputAction* FireAction;
//
// protected:
// 	/** Called for movement input */
// 	void Move(const FInputActionValue& Value);
//
// 	/** Called for looking input */
// 	void Look(const FInputActionValue& Value);
//
// 	/** Called for looking input */
// 	UFUNCTION(BlueprintNativeEvent)
// 	void TryFire(const FInputActionValue& Value);
// 	void TryFire_Implementation(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

