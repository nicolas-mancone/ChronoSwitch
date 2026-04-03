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
	/** Visual indicator displayed when the Past and Future meshes diverge significantly. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Causal Settings|Visuals")
	TObjectPtr<UStaticMeshComponent> GhostMesh;
	
	/** Handles audio feedback for physical impacts and desync events in the Past timeline. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UAudioComponent> PastAudioComp;
 
	/** Handles audio feedback for physical impacts and desync events in the Future timeline. */
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

	/** Maximum acceleration applied by the spring. Caps extreme forces to prevent wall tunneling. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Causal Settings|Physics")
	float MaxAcceleration;

	/** Caps the overall physics velocity to ensure stable collisions at high speeds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Causal Settings|Physics")
	float MaxVelocity;

	/** Determines how smoothly the FutureMesh tracks the PastMesh when handled by a player. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Causal Settings|Interaction")
	float HeldInterpSpeed;

	/** Z-axis distance used to detect if a player is standing on the mesh, preventing physics loops during lifting. */
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

	/** Tracked manually during kinematic movement (when held) to preserve momentum upon release. */
	FVector FutureMeshVelocity;

	// --- Impact Debouncing State ---
	// Used to filter out continuous sliding or multi-hit frame spam, ensuring clean audio triggers.
	float LastImpactTime_Past = 0.0f;
	float LastImpactTime_Future = 0.0f;
	FVector LastImpactNormal_Past = FVector::ZeroVector;
	FVector LastImpactNormal_Future = FVector::ZeroVector;
	
	// --- Desync Optimization State ---
	// Caches the current state to avoid triggering audio/visual parameter updates every single frame.
	bool bIsDesynced = false;
	float LastDesyncDistance = -1.0f;

	/** Authoritative Server transform used to smoothly correct client-side physics drift. */
	UPROPERTY(Replicated)
	FTransform ServerFutureTransform;
};