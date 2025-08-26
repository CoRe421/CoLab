// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollabActor.generated.h"

UCLASS()
class COLLAB_API ACollabActor : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(AllowPrivateAccess))
	bool bSmoothClientPhysics;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(AllowPrivateAccess))
	float NetRelevancyDistance;

public:
	// Sets default values for this actor's properties
	ACollabActor();

	UFUNCTION(BlueprintCallable)
	void SetNetRelevancyDistance(const float Distance);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;

	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void RerunConstructionScripts() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditMove(bool bFinished) override;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPostPropertyChanged(const FName Property);
	virtual void OnPostPropertyChanged_Implementation(const FName Property);
#endif

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPostConstruction();
	virtual void OnPostConstruction_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPostActorUpdate();
	virtual void OnPostActorUpdate_Implementation() {};
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
