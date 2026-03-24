#include "Gameplay/TimelineActors/CausalActor.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Game/ChronoSwitchPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "Physics/PhysicsInterfaceCore.h"
#include "Net/UnrealNetwork.h"
#include "Chaos/ChaosEngineInterface.h"

ACausalActor::ACausalActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// PrePhysics tick ensures passengers resolve movement accurately on moving platforms.
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	NetPriority = 5.0f;
	
	ActorTimeline = EActorTimeline::Both_Causal;

	DesyncThreshold = 50.0f;
	SpringStiffness = 30.0f;    
	SpringDamping = 5.0f;
	MaxPullDistance = 800.0f;
	MaxAcceleration = 8000.0f; 
	MaxVelocity = 1500.0f; 
	HeldInterpSpeed = 20.0f;
	LiftVerticalTolerance = 15.0f;
	InteractedComponent = nullptr;
	FutureMeshVelocity = FVector::ZeroVector;
	InteractingCharacter = nullptr;
	FutureInteractingCharacter = nullptr;
	
	GhostMesh = CreateDefaultSubobject<UStaticMeshComponent>("GhostMesh");
	GhostMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GhostMesh->SetHiddenInGame(true);
	GhostMesh->SetCastShadow(false);

	PastAudioComp = CreateDefaultSubobject<UAudioComponent>("PastAudioComp");
	PastAudioComp->SetupAttachment(RootComponent);
	PastAudioComp->bAutoActivate = false;

	FutureAudioComp = CreateDefaultSubobject<UAudioComponent>("FutureAudioComp");
	FutureAudioComp->SetupAttachment(FutureMesh);
	FutureAudioComp->bAutoActivate = false;
	
	if (PastMesh)
	{
		SetRootComponent(PastMesh);
		
		if (FutureMesh) FutureMesh->SetupAttachment(PastMesh);
		if (GhostMesh) GhostMesh->SetupAttachment(PastMesh);

		PastMesh->SetSimulatePhysics(true);
		PastMesh->SetEnableGravity(true);
		PastMesh->BodyInstance.bUseCCD = true; 
	}
	
	if (FutureMesh)
	{
		FutureMesh->SetSimulatePhysics(true);
		FutureMesh->SetEnableGravity(true);
		FutureMesh->SetIsReplicated(false);
		FutureMesh->BodyInstance.bUseCCD = true; 
	}
}

void ACausalActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACausalActor, FutureInteractingCharacter);
}

void ACausalActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (FutureMesh)
	{
		FutureMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
	
	if (PastMesh) PastMesh->SetSimulatePhysics(true);
	if (FutureMesh) FutureMesh->SetSimulatePhysics(true);
	
	if (PastMesh)
	{
		PastMesh->SetNotifyRigidBodyCollision(true);
		PastMesh->OnComponentHit.AddDynamic(this, &ACausalActor::OnMeshHit);
	}
	if (FutureMesh)
	{
		FutureMesh->SetNotifyRigidBodyCollision(true);
		FutureMesh->OnComponentHit.AddDynamic(this, &ACausalActor::OnMeshHit);
	}
}

void ACausalActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateSlaveMesh(DeltaTime);
	UpdateDesyncState();
}

void ACausalActor::Interact_Implementation(ACharacter* Interactor)
{
}

FText ACausalActor::GetInteractPrompt_Implementation()
{
	const APlayerController* PC = GetWorld()->GetFirstPlayerController();
	const APawn* LocalPawn = PC ? PC->GetPawn() : nullptr;
	
	if (InteractingCharacter == LocalPawn || FutureInteractingCharacter == LocalPawn)
	{
		return FText::FromString("Press F to Release");
	}

	return FText::FromString("Press F to Grab");
}

bool ACausalActor::CanBeGrabbed(UPrimitiveComponent* MeshToGrab) const
{
	if (MeshToGrab == PastMesh)
	{
		return InteractedComponent == nullptr;
	}
	if (MeshToGrab == FutureMesh)
	{
		return FutureInteractingCharacter == nullptr;
	}
	
	return false;
}

void ACausalActor::NotifyOnGrabbed(UPrimitiveComponent* Mesh, ACharacter* Grabber)
{
	if (Mesh == FutureMesh)
	{
		FutureInteractingCharacter = Grabber;
		
		if (Grabber)
		{
			AddTickPrerequisiteActor(Grabber);
		}
	}
	else if (Mesh == PastMesh)
	{
		Super::NotifyOnGrabbed(Mesh, Grabber);

		if (FutureMesh && !FutureInteractingCharacter)
		{
			FutureMesh->SetSimulatePhysics(false);
			FutureMesh->SetEnableGravity(false);
			FutureMeshVelocity = FVector::ZeroVector;
		}
	}
}

void ACausalActor::NotifyOnReleased(UPrimitiveComponent* Mesh, ACharacter* Grabber)
{
	if (Mesh == FutureMesh)
	{
		FutureInteractingCharacter = nullptr;
		if (Grabber)
		{
			RemoveTickPrerequisiteActor(Grabber);
		}
	}
	else if (Mesh == PastMesh)
	{
		Super::NotifyOnReleased(Mesh, Grabber);
	}

	if (FutureMesh && !FutureInteractingCharacter)
	{
		if (InteractedComponent == PastMesh)
		{
			FutureMesh->SetSimulatePhysics(false);
			FutureMesh->SetEnableGravity(false);
			FutureMeshVelocity = FVector::ZeroVector;
		}
		else
		{
			FutureMesh->SetSimulatePhysics(true);
			FutureMesh->SetEnableGravity(true);

			FutureMesh->SetPhysicsLinearVelocity(FutureMeshVelocity);

			if (PastMesh)
			{
				FutureMesh->SetPhysicsAngularVelocityInDegrees(PastMesh->GetPhysicsAngularVelocityInDegrees());
			}
		}
	}
}

void ACausalActor::UpdateSlaveMesh(float DeltaTime)
{
	if (!PastMesh || !FutureMesh) return;

	if (FutureInteractingCharacter) return;

	const FVector TargetLocation = PastMesh->GetComponentLocation();
	const FRotator TargetRotation = PastMesh->GetComponentRotation();

	if (InteractedComponent == PastMesh)
	{
		const FVector CurrentLoc = FutureMesh->GetComponentLocation();
		const FVector MoveDelta = TargetLocation - CurrentLoc;
		const bool bIsLifting = MoveDelta.Z > 0.1f;

		if (bIsLifting)
		{
			if (AGameStateBase* GameState = GetWorld()->GetGameState())
			{
				for (APlayerState* PS : GameState->PlayerArray)
				{
					if (ACharacter* Char = Cast<ACharacter>(PS->GetPawn()))
					{
						const float CharBottomZ = Char->GetActorLocation().Z - Char->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
						const FBoxSphereBounds MeshBounds = FutureMesh->CalcBounds(FutureMesh->GetComponentTransform());
						const float MeshTopZ = MeshBounds.Origin.Z + MeshBounds.BoxExtent.Z;
							
						const bool bIsPhysicallyAbove = CharBottomZ >= (MeshTopZ - LiftVerticalTolerance);

						if (Char->GetMovementBase() == FutureMesh && bIsPhysicallyAbove)
						{
							FutureMesh->IgnoreActorWhenMoving(Char, true);
						}
					}
				}
			}
		}
		
		const FVector NextLoc = FMath::VInterpTo(CurrentLoc, TargetLocation, DeltaTime, HeldInterpSpeed);
		const FRotator NextRot = FMath::RInterpTo(FutureMesh->GetComponentRotation(), TargetRotation, DeltaTime, HeldInterpSpeed);

		FHitResult Hit;
		FutureMesh->SetWorldLocationAndRotation(NextLoc, NextRot, true, &Hit);

		if (DeltaTime > UE_KINDA_SMALL_NUMBER)
		{
			FutureMeshVelocity = (FutureMesh->GetComponentLocation() - CurrentLoc) / DeltaTime;
		}
		
		FutureMesh->ClearMoveIgnoreActors();
	}
	else if (InteractedComponent == nullptr && FutureMesh)
	{
		if (FBodyInstance* BodyInst = FutureMesh->GetBodyInstance())
		{
			float Stiffness = SpringStiffness;
			float Damping = SpringDamping;
			float MaxDist = MaxPullDistance;
			float MaxAccel = MaxAcceleration;
			
			FBodyInstance* MasterBodyInst = PastMesh ? PastMesh->GetBodyInstance() : nullptr;
			FVector FallbackTarget = TargetLocation;
			FQuat FallbackRotation = TargetRotation.Quaternion();

			FCalculateCustomPhysics CalculateCustomPhysics = FCalculateCustomPhysics::CreateLambda([Stiffness, Damping, MaxDist, MaxAccel, MasterBodyInst, FallbackTarget, FallbackRotation](float PhysicsDeltaTime, FBodyInstance* BI)
			{
				if (!BI || !BI->IsValidBodyInstance()) return;

				const FTransform BodyTransform = BI->GetUnrealWorldTransform_AssumesLocked();
				const FVector CurrentLocation = BodyTransform.GetLocation();
				FVector CurrentVelocity = BI->GetUnrealWorldVelocity_AssumesLocked();

				FVector RealTimeTarget = FallbackTarget;
				FQuat RealTimeRotation = FallbackRotation;
				FVector RealTimeLinearVelocity = FVector::ZeroVector;
				FVector RealTimeAngularVelocity = FVector::ZeroVector;

				if (MasterBodyInst && MasterBodyInst->IsValidBodyInstance())
				{
					FTransform MasterTransform = MasterBodyInst->GetUnrealWorldTransform_AssumesLocked();
					RealTimeTarget = MasterTransform.GetLocation();
					RealTimeRotation = MasterTransform.GetRotation();
					RealTimeLinearVelocity = MasterBodyInst->GetUnrealWorldVelocity_AssumesLocked();
					RealTimeAngularVelocity = MasterBodyInst->GetUnrealWorldAngularVelocityInRadians_AssumesLocked();
				}

				FVector Delta = RealTimeTarget - CurrentLocation;
				
				if (Delta.SizeSquared() > MaxDist * MaxDist)
				{
					Delta = Delta.GetSafeNormal() * MaxDist;
				}

				const FVector SpringForce = Delta * Stiffness;
				const FVector DampingForce = -(CurrentVelocity - RealTimeLinearVelocity) * Damping;
				
				const FVector TotalForce = SpringForce + DampingForce;

				const FVector ClampedForce = TotalForce.GetClampedToMaxSize(MaxAccel);

				FPhysicsInterface::AddForce_AssumesLocked(BI->GetPhysicsActorHandle(), ClampedForce, true, true);

				const FQuat CurrentRotation = BodyTransform.GetRotation();
				FQuat ErrorRot = RealTimeRotation * CurrentRotation.Inverse();
				FVector Axis;
				float Angle;
				ErrorRot.ToAxisAndAngle(Axis, Angle);

				if (Angle > UE_PI) Angle -= 2.0f * UE_PI;
				
				const FVector AngularVelocity = BI->GetUnrealWorldAngularVelocityInRadians_AssumesLocked();
				const FVector Torque = (Axis * Angle * Stiffness) - ((AngularVelocity - RealTimeAngularVelocity) * Damping);
				
				FPhysicsInterface::AddTorque_AssumesLocked(BI->GetPhysicsActorHandle(), Torque, true, true);
			});

			BodyInst->AddCustomPhysics(CalculateCustomPhysics);
		}
	}
}

void ACausalActor::UpdateDesyncState()
{
	if (!GhostMesh || !PastMesh || !FutureMesh) return;

	const float Distance = FVector::Dist(PastMesh->GetComponentLocation(), FutureMesh->GetComponentLocation());
	const bool bCurrentlyDesynced = Distance > DesyncThreshold;

	if (bCurrentlyDesynced != bIsDesynced)
	{
		bIsDesynced = bCurrentlyDesynced;
		
		if (bIsDesynced)
		{
			GhostMesh->SetHiddenInGame(false);
			if (PastAudioComp) PastAudioComp->Play();
			if (FutureAudioComp) FutureAudioComp->Play();
			ReceiveOnDesyncStarted();
		}
		else
		{
			GhostMesh->SetHiddenInGame(true);
			if (PastAudioComp) PastAudioComp->FadeOut(0.2f, 0.0f);
			if (FutureAudioComp) FutureAudioComp->FadeOut(0.2f, 0.0f);
			ReceiveOnDesyncEnded();
		}
	}

	if (bIsDesynced)
	{
		GhostMesh->SetWorldLocationAndRotation(PastMesh->GetComponentLocation(), PastMesh->GetComponentRotation());
		
		uint8 LocalTimelineID = 255;
		if (const APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (const AChronoSwitchPlayerState* PS = PC->GetPlayerState<AChronoSwitchPlayerState>())
			{
				LocalTimelineID = PS->GetTimelineID();
			}
		}
		
		if (PastAudioComp) PastAudioComp->SetVolumeMultiplier(LocalTimelineID == 0 ? 1.0f : 0.0f);
		if (FutureAudioComp) FutureAudioComp->SetVolumeMultiplier(LocalTimelineID == 1 ? 1.0f : 0.0f);

		if (FMath::Abs(Distance - LastDesyncDistance) > 2.0f)
		{
			LastDesyncDistance = Distance;
			
			const float Intensity = FMath::Clamp((Distance - DesyncThreshold) / (MaxPullDistance - DesyncThreshold), 0.0f, 1.0f);
			
			if (PastAudioComp) PastAudioComp->SetFloatParameter(FName("DesyncIntensity"), Intensity);
			if (FutureAudioComp) FutureAudioComp->SetFloatParameter(FName("DesyncIntensity"), Intensity);
			
			ReceiveOnDesyncUpdated(Distance, Intensity);
		}
	}
}

void ACausalActor::OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HitComponent) return; 
	if (OtherActor == this) return;
	
	FVector MyVelocity = HitComponent->GetComponentVelocity();
	
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	if (!HitComponent->IsSimulatingPhysics() && Hit.TraceStart != Hit.TraceEnd && DeltaTime > UE_KINDA_SMALL_NUMBER)
	{
		if (Hit.Distance <= 1.0f) return;

		MyVelocity = (Hit.TraceEnd - Hit.TraceStart) / DeltaTime;
	}

	const FVector OtherVelocity = OtherComp ? OtherComp->GetComponentVelocity() : FVector::ZeroVector;
	const FVector RelativeVelocity = MyVelocity - OtherVelocity;
	
	const float ApproachSpeed = -FVector::DotProduct(RelativeVelocity, Hit.ImpactNormal);

	if (ApproachSpeed < 50.0f) return;

	const uint8 MeshTimelineID = (HitComponent == PastMesh) ? 0 : 1;
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	if (MeshTimelineID == 0)
	{
		if (CurrentTime - LastImpactTime_Past < 0.1f) return;
		
		if (CurrentTime - LastImpactTime_Past < 0.4f && FVector::DotProduct(LastImpactNormal_Past, Hit.ImpactNormal) > 0.95f)
		{
			LastImpactTime_Past = CurrentTime; 
			return;
		}

		LastImpactTime_Past = CurrentTime;
		LastImpactNormal_Past = Hit.ImpactNormal;
	}
	else
	{
		if (CurrentTime - LastImpactTime_Future < 0.1f) return;

		if (CurrentTime - LastImpactTime_Future < 0.4f && FVector::DotProduct(LastImpactNormal_Future, Hit.ImpactNormal) > 0.95f)
		{
			LastImpactTime_Future = CurrentTime;
			return;
		}

		LastImpactTime_Future = CurrentTime;
		LastImpactNormal_Future = Hit.ImpactNormal;
	}
	
	if (const APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (const AChronoSwitchPlayerState* PS = PC->GetPlayerState<AChronoSwitchPlayerState>())
		{
			if (PS->GetTimelineID() == MeshTimelineID)
			{
				ReceiveOnMeshImpact(Hit.ImpactPoint, ApproachSpeed);
			}
		}
	}
}
