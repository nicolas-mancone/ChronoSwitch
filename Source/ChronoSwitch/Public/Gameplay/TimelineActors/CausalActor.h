// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsTimelineActor.h"
#include "CausalActor.generated.h"

/**
 * A physics actor existing simultaneously in two timelines. 
 * Relies on a Master (Past) to Slave (Future) relationship, supporting kinematic syncing when held and asynchronous physics springs when released.
 */
UCLASS(meta = (PrioritizeCategories = "Causal Settings"))
class CHRONOSWITCH_API ACausalActor : public APhysicsTimelineActor
{
	GENERATED_BODY()

public:
	ACausalActor();
	
	virtual void Interact_Implementation(ACharacter* Interactor) override;
	virtual FText GetInteractPrompt_Implementation() override;

	virtual void NotifyOnGrabbed(UPrimitiveComponent* Mesh, ACharacter* Grabber) override;
	virtual void NotifyOnReleased(UPrimitiveComponent* Mesh, ACharacter* Grabber) override;

	virtual bool CanBeGrabbed(UPrimitiveComponent* MeshToGrab) const override;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	/** Visual mesh shown when Past and Future meshes desynchronize. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Causal Settings|Visuals")
	TObjectPtr<UStaticMeshComponent> GhostMesh;
	
	/** Audio component attached to the PastMesh. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UAudioComponent> PastAudioComp;
 
	/** Audio component attached to the FutureMesh. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UAudioComponent> FutureAudioComp;

	/** Distance threshold before the desync effect (ghost mesh, audio) activates. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Causal Settings|Visuals")
	float DesyncThreshold;
	
	/** Strength of the spring acceleration pulling the FutureMesh to the PastMesh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Causal Settings|Physics")
	float SpringStiffness;

	/** Damping factor to prevent spring oscillation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Causal Settings|Physics")
	float SpringDamping;

	/** Maximum distance considered for spring force calculation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Causal Settings|Physics")
	float MaxPullDistance;

	/** Maximum acceleration applied by the spring to prevent wall tunneling. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Causal Settings|Physics")
	float MaxAcceleration;

	/** Maximum velocity limit to ensure stable physics collisions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Causal Settings|Physics")
	float MaxVelocity;

	/** Interpolation speed for kinematic syncing when the mesh is held. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Causal Settings|Interaction")
	float HeldInterpSpeed;

	/** Vertical tolerance to detect players standing on the mesh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Causal Settings|Interaction")
	float LiftVerticalTolerance;

	/** Tracks the character holding the FutureMesh independently of the PastMesh. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Causal State")
	TObjectPtr<ACharacter> FutureInteractingCharacter;
	
	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Triggered on impact if the local player is in the same timeline as the hitting mesh. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Causal Settings|Audio")
	void ReceiveOnMeshImpact(FVector ImpactLocation, float ImpactForce);
	
	/** Triggered when meshes separate beyond the DesyncThreshold. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Causal Settings|Audio")
	void ReceiveOnDesyncStarted();
 
	/** Triggered when meshes return within the DesyncThreshold. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Causal Settings|Audio")
	void ReceiveOnDesyncEnded();
 
	/** Continuously triggered while desynced. Intensity scales from 0.0 to 1.0 based on MaxPullDistance. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Causal Settings|Audio")
	void ReceiveOnDesyncUpdated(float Distance, float Intensity);

private:
	void UpdateSlaveMesh(float DeltaTime);

	void UpdateDesyncState();

	FVector FutureMeshVelocity;

	float LastImpactTime_Past = 0.0f;

	float LastImpactTime_Future = 0.0f;

	FVector LastImpactNormal_Past = FVector::ZeroVector;

	FVector LastImpactNormal_Future = FVector::ZeroVector;
	
	bool bIsDesynced = false;
 
	float LastDesyncDistance = -1.0f;
};