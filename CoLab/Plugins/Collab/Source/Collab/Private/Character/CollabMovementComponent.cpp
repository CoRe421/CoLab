// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CollabMovementComponent.h"

#include "CollabLog.h"
#include "Character/CollabCharacterBase.h"
#include "GameplayAbilitySystem/CollabAbilitySystemComponent.h"
#include "GameplayAbilitySystem/Attributes/CollabMovementAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UCollabMovementComponent::UCollabMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	SetIsReplicatedByDefault(true);

	// const UCollabMovementAttributeSet* DefaultMovementValues = GetDefault<UCollabMovementAttributeSet>();
	// if (IsValid(DefaultMovementValues))
	// {
	// 	MaxWalkSpeed = DefaultMovementValues->GetMovementSpeed();
	// 	MaxAcceleration = DefaultMovementValues->GetAcceleration();
	// 	bAllowSlidingWhileMoving = DefaultMovementValues->GetbAllowSlidingWhileMoving() > 0;
	// 	LandingFrictionGracePeriod = DefaultMovementValues->GetLandingFrictionGracePeriod();
	// 	TargetJumpHeight = DefaultMovementValues->GetJumpHeight();
	// 	Mass = DefaultMovementValues->GetMass();
	// 	GravityScale = DefaultMovementValues->GetGravityScale();
	// 	AirControl = DefaultMovementValues->GetAirControl();
	// 	MaxAirSpeed = DefaultMovementValues->GetAirSpeed();
	// 	AirAcceleration = DefaultMovementValues->GetAirAcceleration();
	// 	AirControlBoostMultiplier = DefaultMovementValues->GetAirControlBoostMultiplier();
	// 	AirControlBoostVelocityThreshold = DefaultMovementValues->GetAirControlBoostVelocityThreshold();
	// 	FallingLateralFriction = DefaultMovementValues->GetFallingLateralFriction();
	// }

	// This made the player constantly move when not pressing input - CR
	// bForceMaxAccel = true;
}


// Called when the game starts
void UCollabMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UCollabMovementComponent::OnRegister()
{
	Super::OnRegister();
	
	AActor* Owner = GetOwner();
	check(Owner);

	ACollabCharacterBase* CollabCharacterOwner = Cast<ACollabCharacterBase>(Owner);
	if (IsValid(CollabCharacterOwner))
	{
		CollabCharacterOwner->OnAbilitySystemInit.AddUniqueDynamic(this, &ThisClass::InitializeWithAbilitySystem);
	}
}

void UCollabMovementComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();
	
	Super::OnUnregister();
}


// Called every frame
void UCollabMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCollabMovementComponent::InitializeWithAbilitySystem(UCollabAbilitySystemComponent* InASC)
{
	if (!ensureAlways(IsValid(InASC)))
	{
		return;
	}
	
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent.IsValid())
	{
		UE_LOG(LogCollab, Error, TEXT("CollabMovementComponent: Movement component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent.IsValid())
	{
		UE_LOG(LogCollab, Error, TEXT("CollabMovementComponent: Cannot initialize movement component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	const UCollabMovementAttributeSet* MovementSet = AbilitySystemComponent->GetSet<UCollabMovementAttributeSet>();
	if (IsValid(MovementSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCollabMovementAttributeSet::GetMovementSpeedAttribute()).AddUObject(this, &ThisClass::OnMovementSpeedChanged);
		OnMovementSpeedChanged(FOnCollabInitialAttributeChangeData(UCollabMovementAttributeSet::GetMovementSpeedAttribute(), MovementSet));
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCollabMovementAttributeSet::GetAccelerationAttribute()).AddUObject(this, &ThisClass::OnGroundAccelerationChanged);
		OnGroundAccelerationChanged(FOnCollabInitialAttributeChangeData(UCollabMovementAttributeSet::GetAccelerationAttribute(), MovementSet));
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCollabMovementAttributeSet::GetGroundFrictionAttribute()).AddUObject(this, &ThisClass::OnGroundFrictionChanged);
		OnGroundFrictionChanged(FOnCollabInitialAttributeChangeData(UCollabMovementAttributeSet::GetGroundFrictionAttribute(), MovementSet));
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCollabMovementAttributeSet::GetbAllowSlidingWhileMovingAttribute()).AddUObject(this, &ThisClass::OnAllowSlidingWhileMovingChanged);
		OnAllowSlidingWhileMovingChanged(FOnCollabInitialAttributeChangeData(UCollabMovementAttributeSet::GetbAllowSlidingWhileMovingAttribute(), MovementSet));
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCollabMovementAttributeSet::GetLandingFrictionGracePeriodAttribute()).AddUObject(this, &ThisClass::OnLandingFrictionGracePeriodChanged);
		OnLandingFrictionGracePeriodChanged(FOnCollabInitialAttributeChangeData(UCollabMovementAttributeSet::GetLandingFrictionGracePeriodAttribute(), MovementSet));
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCollabMovementAttributeSet::GetJumpHeightAttribute()).AddUObject(this, &ThisClass::OnJumpHeightChanged);
		OnJumpHeightChanged(FOnCollabInitialAttributeChangeData(UCollabMovementAttributeSet::GetJumpHeightAttribute(), MovementSet));
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCollabMovementAttributeSet::GetMassAttribute()).AddUObject(this, &ThisClass::OnMassChanged);
		OnMassChanged(FOnCollabInitialAttributeChangeData(UCollabMovementAttributeSet::GetMassAttribute(), MovementSet));
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCollabMovementAttributeSet::GetGravityScaleAttribute()).AddUObject(this, &ThisClass::OnGravityScaleChanged);
		OnGravityScaleChanged(FOnCollabInitialAttributeChangeData(UCollabMovementAttributeSet::GetGravityScaleAttribute(), MovementSet));
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCollabMovementAttributeSet::GetAirControlAttribute()).AddUObject(this, &ThisClass::OnAirControlChanged);
		OnAirControlChanged(FOnCollabInitialAttributeChangeData(UCollabMovementAttributeSet::GetAirControlAttribute(), MovementSet));
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCollabMovementAttributeSet::GetAirSpeedAttribute()).AddUObject(this, &ThisClass::OnAirSpeedChanged);
		OnAirSpeedChanged(FOnCollabInitialAttributeChangeData(UCollabMovementAttributeSet::GetAirSpeedAttribute(), MovementSet));
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCollabMovementAttributeSet::GetAirAccelerationAttribute()).AddUObject(this, &ThisClass::OnAirAccelerationChanged);
		OnAirAccelerationChanged(FOnCollabInitialAttributeChangeData(UCollabMovementAttributeSet::GetAirAccelerationAttribute(), MovementSet));
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCollabMovementAttributeSet::GetAirControlBoostMultiplierAttribute()).AddUObject(this, &ThisClass::OnAirControlBoostMultiplierChanged);
		OnAirControlBoostMultiplierChanged(FOnCollabInitialAttributeChangeData(UCollabMovementAttributeSet::GetAirControlBoostMultiplierAttribute(), MovementSet));
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCollabMovementAttributeSet::GetAirControlBoostVelocityThresholdAttribute()).AddUObject(this, &ThisClass::OnAirControlBoostVelocityThresholdChanged);
		OnAirControlBoostVelocityThresholdChanged(FOnCollabInitialAttributeChangeData(UCollabMovementAttributeSet::GetAirControlBoostVelocityThresholdAttribute(), MovementSet));
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCollabMovementAttributeSet::GetFallingLateralFrictionAttribute()).AddUObject(this, &ThisClass::OnFallingLateralFrictionChanged);
		OnFallingLateralFrictionChanged(FOnCollabInitialAttributeChangeData(UCollabMovementAttributeSet::GetFallingLateralFrictionAttribute(), MovementSet));
		
		OnJumpValuesUpdated();
	}
	else
	{
		UE_LOG(LogCollab, Log, TEXT("CollabMovementComponent: No movement set found on owner [%s]."), *GetNameSafe(Owner));
	}
}

void UCollabMovementComponent::UninitializeFromAbilitySystem()
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->RemoveAttributeSetBindingsFromObject(UCollabMovementAttributeSet::StaticClass(), this);
	}
	
	AbilitySystemComponent = nullptr;
}

float UCollabMovementComponent::GetMaxAcceleration() const
{
	if (MovementMode == MOVE_Falling)
	{
		return AirAcceleration;
	}
	
	return MaxAcceleration;
}

void UCollabMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{
	// Internal movement physics - CR
	// For walking this mostly amounts to quicker strafing
	Phys_Internal(deltaTime);

	// Nothing from UCharacterMovementComponent, conflicts with internal movement so we can call it - CR
	Super::PhysWalking(deltaTime, Iterations);
}

void UCollabMovementComponent::PhysFalling(float deltaTime, int32 Iterations)
{
	// Internal movement physics - CR
	// For falling this allows for surfing / air strafing / bunny hopping / quick reversal of air momentum
	Phys_Internal(deltaTime);

	// Code from UCharacterMovementComponent, with certain parts removed which conflicted with internal movement physics - CR
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}
	
	FVector FallAcceleration = GetFallingLateralAcceleration(deltaTime);
	const FVector GravityRelativeFallAcceleration = RotateWorldToGravity(FallAcceleration);
	FallAcceleration = RotateGravityToWorld(FVector(GravityRelativeFallAcceleration.X, GravityRelativeFallAcceleration.Y, 0));
	const bool bHasLimitedAirControl = ShouldLimitAirControl(deltaTime, FallAcceleration);
	
	float remainingTime = deltaTime;
	while( (remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) )
	{
		Iterations++;
		float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;
		
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FQuat PawnRotation = UpdatedComponent->GetComponentQuat();
		
		// bJustTeleported = false;
		//
		const FVector OldVelocityWithRootMotion = Velocity;
		//
		// RestorePreAdditiveRootMotionVelocity();
	
		// const FVector OldVelocity = Velocity;
	
		// Apply input
		// const float MaxDecel = GetMaxBrakingDeceleration();
		// if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		// {
		// 	// Compute Velocity
		// 	{
		// 		// Acceleration = FallAcceleration for CalcVelocity(), but we restore it after using it.
		// 		// TGuardValue<FVector> RestoreAcceleration(Acceleration, FallAcceleration);
		// 		// if (HasCustomGravity())
		// 		// {
		// 		// 	Velocity = FVector::VectorPlaneProject(Velocity, RotateGravityToWorld(FVector::UpVector));
		// 		// 	const FVector GravityRelativeOffset = OldVelocity - Velocity;
		// 		// 	CalcVelocity(timeTick, FallingLateralFriction, false, 0.f);
		// 		// 	Velocity += GravityRelativeOffset;
		// 		// }
		// 		// else
		// 		// {
		// 			// Velocity.Z = 0.f;
		// 			// CalcVelocity(timeTick, FallingLateralFriction, false, 0.f);
		// 			// Velocity.Z = OldVelocity.Z;
		// 		// }
		// 	}
		// }
		//
		// Compute current gravity
		const FVector Gravity = -GetGravityDirection() * GetGravityZ();
		float GravityTime = timeTick;
		
		// If jump is providing force, gravity may be affected.
		bool bEndingJumpForce = false;
		if (CharacterOwner->JumpForceTimeRemaining > 0.0f)
		{
			// Consume some of the force time. Only the remaining time (if any) is affected by gravity when bApplyGravityWhileJumping=false.
			const float JumpForceTime = FMath::Min(CharacterOwner->JumpForceTimeRemaining, timeTick);
			GravityTime = bApplyGravityWhileJumping ? timeTick : FMath::Max(0.0f, timeTick - JumpForceTime);
			
			// Update Character state
			CharacterOwner->JumpForceTimeRemaining -= JumpForceTime;
			if (CharacterOwner->JumpForceTimeRemaining <= 0.0f)
			{
				CharacterOwner->ResetJumpState();
				bEndingJumpForce = true;
			}
		}
	
		// Apply gravity
		Velocity = NewFallVelocity(Velocity, Gravity, GravityTime);
	
		//UE_LOG(LogCharacterMovement, Log, TEXT("dt=(%.6f) OldLocation=(%s) OldVelocity=(%s) OldVelocityWithRootMotion=(%s) NewVelocity=(%s)"), timeTick, *(UpdatedComponent->GetComponentLocation()).ToString(), *OldVelocity.ToString(), *OldVelocityWithRootMotion.ToString(), *Velocity.ToString());
		// ApplyRootMotionToVelocity(timeTick);
		// DecayFormerBaseVelocity(timeTick);
	
		// // See if we need to sub-step to exactly reach the apex. This is important for avoiding "cutting off the top" of the trajectory as framerate varies.
		// const FVector GravityRelativeOldVelocityWithRootMotion = RotateWorldToGravity(OldVelocityWithRootMotion);
		// if (CharacterMovementCVars::ForceJumpPeakSubstep && GravityRelativeOldVelocityWithRootMotion.Z > 0.f && RotateWorldToGravity(Velocity).Z <= 0.f && NumJumpApexAttempts < MaxJumpApexAttemptsPerSimulation)
		// {
		// 	const FVector DerivedAccel = (Velocity - OldVelocityWithRootMotion) / timeTick;
		// 	const FVector GravityRelativeDerivedAccel = RotateWorldToGravity(DerivedAccel);
		// 	if (!FMath::IsNearlyZero(GravityRelativeDerivedAccel.Z))
		// 	{
		// 		const float TimeToApex = -GravityRelativeOldVelocityWithRootMotion.Z / GravityRelativeDerivedAccel.Z;
		// 		
		// 		// The time-to-apex calculation should be precise, and we want to avoid adding a substep when we are basically already at the apex from the previous iteration's work.
		// 		const float ApexTimeMinimum = 0.0001f;
		// 		if (TimeToApex >= ApexTimeMinimum && TimeToApex < timeTick)
		// 		{
		// 			const FVector ApexVelocity = OldVelocityWithRootMotion + (DerivedAccel * TimeToApex);
		// 			if (HasCustomGravity())
		// 			{
		// 				const FVector GravityRelativeApexVelocity = RotateWorldToGravity(ApexVelocity);
		// 				Velocity = RotateGravityToWorld(FVector(GravityRelativeApexVelocity.X, GravityRelativeApexVelocity.Y, 0)); // Should be nearly zero anyway, but this makes apex notifications consistent.
		// 			}
		// 			else
		// 			{
		// 				Velocity = ApexVelocity;
		// 				Velocity.Z = 0.f; // Should be nearly zero anyway, but this makes apex notifications consistent.
		// 			}
		// 			
		// 			// We only want to move the amount of time it takes to reach the apex, and refund the unused time for next iteration.
		// 			const float TimeToRefund = (timeTick - TimeToApex);
		//
		// 			remainingTime += TimeToRefund;
		// 			timeTick = TimeToApex;
		// 			Iterations--;
		// 			NumJumpApexAttempts++;
		//
		// 			// Refund time to any active Root Motion Sources as well
		// 			for (TSharedPtr<FRootMotionSource> RootMotionSource : CurrentRootMotion.RootMotionSources)
		// 			{
		// 				const float RewoundRMSTime = FMath::Max(0.0f, RootMotionSource->GetTime() - TimeToRefund);
		// 				RootMotionSource->SetTime(RewoundRMSTime);
		// 			}
		// 		}
		// 	}
		// }
	
		if (bNotifyApex && (RotateWorldToGravity(Velocity).Z < 0.f))
		{
			// Just passed jump apex since now going down
			bNotifyApex = false;
			NotifyJumpApex();
		}
	
		// Compute change in position (using midpoint integration method).
		FVector Adjusted = 0.5f * (OldVelocityWithRootMotion + Velocity) * timeTick;
		
		// Special handling if ending the jump force where we didn't apply gravity during the jump.
		if (bEndingJumpForce && !bApplyGravityWhileJumping)
		{
			// We had a portion of the time at constant speed then a portion with acceleration due to gravity.
			// Account for that here with a more correct change in position.
			const float NonGravityTime = FMath::Max(0.f, timeTick - GravityTime);
			Adjusted = (OldVelocityWithRootMotion * NonGravityTime) + (0.5f*(OldVelocityWithRootMotion + Velocity) * GravityTime);
		}
	
		// Move
		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent( Adjusted, PawnRotation, true, Hit);
		
		if (!HasValidData())
		{
			return;
		}
		
		float LastMoveTimeSlice = timeTick;
		float subTimeTickRemaining = timeTick * (1.f - Hit.Time);
		
		// if ( IsSwimming() ) //just entered water
		// {
		// 	remainingTime += subTimeTickRemaining;
		// 	StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
		// 	return;
		// }
		/*else*/ if ( Hit.bBlockingHit )
		{
			if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit))
			{
				remainingTime += subTimeTickRemaining;
				ProcessLanded(Hit, remainingTime, Iterations);
				return;
			}
			else
			{
				// Compute impact deflection based on final velocity, not integration step.
				// This allows us to compute a new velocity from the deflected vector, and ensures the full gravity effect is included in the slide result.
				Adjusted = Velocity * timeTick;
	
				// See if we can convert a normally invalid landing spot (based on the hit result) to a usable one.
				if (!Hit.bStartPenetrating && ShouldCheckForValidLandingSpot(timeTick, Adjusted, Hit))
				{
					const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
					FFindFloorResult FloorResult;
					FindFloor(PawnLocation, FloorResult, false);
					if (FloorResult.IsWalkableFloor() && IsValidLandingSpot(PawnLocation, FloorResult.HitResult))
					{
						remainingTime += subTimeTickRemaining;
						ProcessLanded(FloorResult.HitResult, remainingTime, Iterations);
						return;
					}
				}
	
				HandleImpact(Hit, LastMoveTimeSlice, Adjusted);
				
				// If we've changed physics mode, abort.
				if (!HasValidData() || !IsFalling())
				{
					return;
				}
	
				// Limit air control based on what we hit.
				// We moved to the impact point using air control, but may want to deflect from there based on a limited air control acceleration.
				// FVector VelocityNoAirControl = OldVelocity;
				// FVector AirControlAccel = Acceleration;
				// if (bHasLimitedAirControl)
				// {
				// 	// Compute VelocityNoAirControl
				// 	{
				// 		// Find velocity *without* acceleration.
				// 		TGuardValue<FVector> RestoreAcceleration(Acceleration, FVector::ZeroVector);
				// 		TGuardValue<FVector> RestoreVelocity(Velocity, OldVelocity);
				// 		if (HasCustomGravity())
				// 		{
				// 			Velocity = FVector::VectorPlaneProject(Velocity, RotateGravityToWorld(FVector::UpVector));
				// 			const FVector GravityRelativeOffset = OldVelocity - Velocity;
				// 			CalcVelocity(timeTick, FallingLateralFriction, false, MaxDecel);
				// 			VelocityNoAirControl = Velocity + GravityRelativeOffset;
				// 		}
				// 		else
				// 		{
				// 			Velocity.Z = 0.f;
				// 			CalcVelocity(timeTick, FallingLateralFriction, false, MaxDecel);
				// 			VelocityNoAirControl = FVector(Velocity.X, Velocity.Y, OldVelocity.Z);
				// 		}
				// 		
				// 		VelocityNoAirControl = NewFallVelocity(VelocityNoAirControl, Gravity, GravityTime);
				// 	}
				//
				// 	const bool bCheckLandingSpot = false; // we already checked above.
				// 	AirControlAccel = (Velocity - VelocityNoAirControl) / timeTick;
				// 	const FVector AirControlDeltaV = LimitAirControl(LastMoveTimeSlice, AirControlAccel, Hit, bCheckLandingSpot) * LastMoveTimeSlice;
				// 	Adjusted = (VelocityNoAirControl + AirControlDeltaV) * LastMoveTimeSlice;
				// }
	
				const FVector OldHitNormal = Hit.Normal;
				const FVector OldHitImpactNormal = Hit.ImpactNormal;				
				FVector Delta = ComputeSlideVector(Adjusted, 1.f - Hit.Time, OldHitNormal, Hit);
	
				// Compute velocity after deflection (only gravity component for RootMotion)
				const UPrimitiveComponent* HitComponent = Hit.GetComponent();
				// if (CharacterMovementCVars::UseTargetVelocityOnImpact && !Velocity.IsNearlyZero() && MovementBaseUtility::IsSimulatedBase(HitComponent))
				// {
				// 	const FVector ContactVelocity = MovementBaseUtility::GetMovementBaseVelocity(HitComponent, NAME_None) + MovementBaseUtility::GetMovementBaseTangentialVelocity(HitComponent, NAME_None, Hit.ImpactPoint);
				// 	const FVector NewVelocity = Velocity - Hit.ImpactNormal * FVector::DotProduct(Velocity - ContactVelocity, Hit.ImpactNormal);
				// 	Velocity = HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocityWithIgnoreZAccumulate() ? FVector(Velocity.X, Velocity.Y, NewVelocity.Z) : NewVelocity;
				// }
				/*else*/ if (subTimeTickRemaining > UE_KINDA_SMALL_NUMBER && !bJustTeleported)
				{
					const FVector NewVelocity = (Delta / subTimeTickRemaining);
					Velocity = HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocityWithIgnoreZAccumulate() ? FVector(Velocity.X, Velocity.Y, NewVelocity.Z) : NewVelocity;
				}
				
				if (subTimeTickRemaining > UE_KINDA_SMALL_NUMBER && (Delta | Adjusted) > 0.f)
				{
					// Move in deflected direction.
					SafeMoveUpdatedComponent( Delta, PawnRotation, true, Hit);
					
					if (Hit.bBlockingHit)
					{
						// hit second wall
						LastMoveTimeSlice = subTimeTickRemaining;
						subTimeTickRemaining = subTimeTickRemaining * (1.f - Hit.Time);
				
						if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit))
						{
							remainingTime += subTimeTickRemaining;
							ProcessLanded(Hit, remainingTime, Iterations);
							return;
						}
				
						HandleImpact(Hit, LastMoveTimeSlice, Delta);
				
						// If we've changed physics mode, abort.
						if (!HasValidData() || !IsFalling())
						{
							return;
						}
				
						// // Act as if there was no air control on the last move when computing new deflection.
						// if (bHasLimitedAirControl && RotateWorldToGravity(Hit.Normal).Z > CharacterMovementConstants::VERTICAL_SLOPE_NORMAL_Z)
						// {
						// 	const FVector LastMoveNoAirControl = VelocityNoAirControl * LastMoveTimeSlice;
						// 	Delta = ComputeSlideVector(LastMoveNoAirControl, 1.f, OldHitNormal, Hit);
						// }
				
						FVector PreTwoWallDelta = Delta;
						TwoWallAdjust(Delta, Hit, OldHitNormal);
				
						// // Limit air control, but allow a slide along the second wall.
						// if (bHasLimitedAirControl)
						// {
						// 	const bool bCheckLandingSpot = false; // we already checked above.
						// 	const FVector AirControlDeltaV = LimitAirControl(subTimeTickRemaining, AirControlAccel, Hit, bCheckLandingSpot) * subTimeTickRemaining;
						//
						// 	// Only allow if not back in to first wall
						// 	if (FVector::DotProduct(AirControlDeltaV, OldHitNormal) > 0.f)
						// 	{
						// 		Delta += (AirControlDeltaV * subTimeTickRemaining);
						// 	}
						// }
				
						// Compute velocity after deflection (only gravity component for RootMotion)
						// if (subTimeTickRemaining > UE_KINDA_SMALL_NUMBER && !bJustTeleported)
						// {
						// 	const FVector NewVelocity = (Delta / subTimeTickRemaining);
						// 	Velocity = HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocityWithIgnoreZAccumulate() ? FVector(Velocity.X, Velocity.Y, NewVelocity.Z) : NewVelocity;
						// }
				
						// bDitch=true means that pawn is straddling two slopes, neither of which it can stand on
						bool bDitch = ( (RotateWorldToGravity(OldHitImpactNormal).Z > 0.f) && (RotateWorldToGravity(Hit.ImpactNormal).Z > 0.f) && (FMath::Abs(Delta.Z) <= UE_KINDA_SMALL_NUMBER) && ((Hit.ImpactNormal | OldHitImpactNormal) < 0.f) );
						SafeMoveUpdatedComponent( Delta, PawnRotation, true, Hit);
						if ( Hit.Time == 0.f )
						{
							// if we are stuck then try to side step
							FVector SideDelta = (OldHitNormal + Hit.ImpactNormal).GetSafeNormal2D();
							if ( SideDelta.IsNearlyZero() )
							{
								SideDelta = FVector(OldHitNormal.Y, -OldHitNormal.X, 0).GetSafeNormal();
							}
							SafeMoveUpdatedComponent( SideDelta, PawnRotation, true, Hit);
						}
							
						if ( bDitch || IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit) || Hit.Time == 0.f  )
						{
							remainingTime = 0.f;
							ProcessLanded(Hit, remainingTime, Iterations);
							return;
						}
						// else if (GetPerchRadiusThreshold() > 0.f && Hit.Time == 1.f && RotateWorldToGravity(OldHitImpactNormal).Z >= WalkableFloorZ)
						// {
						// 	// We might be in a virtual 'ditch' within our perch radius. This is rare.
						// 	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
						// 	const float ZMovedDist = FMath::Abs(RotateWorldToGravity(PawnLocation - OldLocation).Z);
						// 	const float MovedDist2DSq = FVector::VectorPlaneProject(PawnLocation - OldLocation, RotateGravityToWorld(FVector::UpVector)).Size2D();
						// 	if (ZMovedDist <= 0.2f * timeTick && MovedDist2DSq <= 4.f * timeTick)
						// 	{
						// 		FVector GravityRelativeVelocity = RotateWorldToGravity(Velocity);
						// 		GravityRelativeVelocity.X += 0.25f * GetMaxSpeed() * (RandomStream.FRand() - 0.5f);
						// 		GravityRelativeVelocity.Y += 0.25f * GetMaxSpeed() * (RandomStream.FRand() - 0.5f);
						// 		GravityRelativeVelocity.Z = FMath::Max<float>(JumpZVelocity * 0.25f, 1.f);
						// 		Velocity = RotateGravityToWorld(GravityRelativeVelocity);
						// 		Delta = Velocity * timeTick;
						// 		SafeMoveUpdatedComponent(Delta, PawnRotation, true, Hit);
						// 	}
						// }
					}
				}
			}
		}
	
		// FVector GravityRelativeVelocity = RotateWorldToGravity(Velocity);
		// if (GravityRelativeVelocity.SizeSquared2D() <= UE_KINDA_SMALL_NUMBER * 10.f)
		// {
		// 	GravityRelativeVelocity.X = 0.f;
		// 	GravityRelativeVelocity.Y = 0.f;
		// 	Velocity = RotateGravityToWorld(GravityRelativeVelocity);
		// }
	}
}

FVector UCollabMovementComponent::ComputeSlideVector(const FVector& Delta, const float Time, const FVector& Normal,
	const FHitResult& Hit) const
{
	// This is code straight from the inherited UMovementComponent, but we want to get around the logic in UCharacterMovementComponent which prevents sliding up slopes - CR
	if (!bConstrainToPlane)
	{
		return FVector::VectorPlaneProject(Delta, Normal) * Time;
	}
	else
	{
		const FVector ProjectedNormal = ConstrainNormalToPlane(Normal);
		return FVector::VectorPlaneProject(Delta, ProjectedNormal) * Time;
	}
}

void UCollabMovementComponent::ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration)
{
	if (LastLandedTime.IsSet())
	{
		const FTimespan TimeSinceFall = FDateTime::Now() - LastLandedTime.GetValue();
		if (TimeSinceFall < FTimespan::FromSeconds(LandingFrictionGracePeriod))
		{
			return;
		}
	}
	
	Super::ApplyVelocityBraking(DeltaTime, Friction, BrakingDeceleration);
}

void UCollabMovementComponent::SetPostLandedPhysics(const FHitResult& Hit)
{
	Super::SetPostLandedPhysics(Hit);

	if (MovementMode != MOVE_Walking)
	{
		return;
	}

	LastLandedTime = FDateTime::Now();
}

void UCollabMovementComponent::OnJumpValuesUpdated()
{
	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}
	
	const AActor* Owner = AbilitySystemComponent->GetOwner();
	if (!ensureAlways(IsValid(Owner)))
	{
		return;
	}
	const UWorld* World = Owner->GetWorld();
	if (!ensureAlways(IsValid(World)))
	{
		return;
	}
	const float Gravity = World->GetGravityZ();
	const float OwnerGravity = Gravity * GravityScale;

	// x == JumpInitialVelocity
	// JumpHeight = -x/2 * x/g = x(-1/2 * 1/g)
	// JumpHeight = x(-1/2g)
	// x = -2g(JumpHeight)
	const float NewJumpVelocity = UKismetMathLibrary::Sqrt(-2 * OwnerGravity * TargetJumpHeight);

	if (NewJumpVelocity != JumpZVelocity)
	{
		UE_LOG(LogCollab, Log, TEXT("New jump velocity: %f"), NewJumpVelocity);
		JumpZVelocity = NewJumpVelocity;
	}
}

void UCollabMovementComponent::Phys_Internal(float DeltaTime)
{
	// Get Input from acceleration, because input is NOT accessible on server
	// IMPORTANT: This must run before gravity / friction is applied to acceleration
	FVector InputVector = Acceleration.GetSafeNormal();
	if (!ensureAlways(InputVector.Z == 0.f))
	{
		InputVector.Z = 0.f;
	}
	
	// # Apply gravity, jumping, and ground friction to velocity
	// velocity.y += gravity * delta
	// if is_on_floor():
	// 	// # By using is_action_pressed() rather than is_action_just_pressed()
	// 	// # we get automatic bunny hopping.
	// 	if Input.is_action_pressed("move_jump"):
	// 		velocity.y = jumpImpulse
	// 	else:
	// 		velocity *= groundFriction
    
	// # Figure out which strafe force and speed limit applies
	float StrafeAcceleration = GetMaxAcceleration();
	float SpeedLimit = MovementMode == MOVE_Falling ? MaxAirSpeed : MaxWalkSpeed;
	
	// # Project current velocity onto the strafe direction, and compute a capped
	// # acceleration such that *projected* speed will remain within the limit.
	float CurrentSpeed = InputVector.Dot(Velocity);
	float TempAcceleration = StrafeAcceleration * DeltaTime;
	TempAcceleration = FMath::Max(0, FMath::Min(TempAcceleration, SpeedLimit - CurrentSpeed));
	Velocity += InputVector * TempAcceleration;
}

void UCollabMovementComponent::OnMovementSpeedChanged(const FOnAttributeChangeData& Data)
{
	MaxWalkSpeed = Data.NewValue;
}

void UCollabMovementComponent::OnGroundAccelerationChanged(const FOnAttributeChangeData& Data)
{
	MaxAcceleration = Data.NewValue;
}

void UCollabMovementComponent::OnGroundFrictionChanged(const FOnAttributeChangeData& Data)
{
	GroundFriction = Data.NewValue;
}

void UCollabMovementComponent::OnAllowSlidingWhileMovingChanged(const FOnAttributeChangeData& Data)
{
	bAllowSlidingWhileMoving = Data.NewValue > 0.f;
}

void UCollabMovementComponent::OnLandingFrictionGracePeriodChanged(const FOnAttributeChangeData& Data)
{
	LandingFrictionGracePeriod = Data.NewValue;
}

void UCollabMovementComponent::OnJumpHeightChanged(const FOnAttributeChangeData& Data)
{
	TargetJumpHeight = Data.NewValue;
	OnJumpValuesUpdated();
}

void UCollabMovementComponent::OnMassChanged(const FOnAttributeChangeData& Data)
{
	Mass = Data.NewValue;
}

void UCollabMovementComponent::OnGravityScaleChanged(const FOnAttributeChangeData& Data)
{
	GravityScale = Data.NewValue;
	OnJumpValuesUpdated();
}

void UCollabMovementComponent::OnAirControlChanged(const FOnAttributeChangeData& Data)
{
	AirControl = Data.NewValue;
}

void UCollabMovementComponent::OnAirSpeedChanged(const FOnAttributeChangeData& Data)
{
	MaxAirSpeed = Data.NewValue;
}

void UCollabMovementComponent::OnAirAccelerationChanged(const FOnAttributeChangeData& Data)
{
	AirAcceleration = Data.NewValue;
}

void UCollabMovementComponent::OnAirControlBoostMultiplierChanged(const FOnAttributeChangeData& Data)
{
	AirControlBoostMultiplier = Data.NewValue;
}

void UCollabMovementComponent::OnAirControlBoostVelocityThresholdChanged(const FOnAttributeChangeData& Data)
{
	AirControlBoostVelocityThreshold = Data.NewValue;
}

void UCollabMovementComponent::OnFallingLateralFrictionChanged(const FOnAttributeChangeData& Data)
{
	FallingLateralFriction = Data.NewValue;
}

