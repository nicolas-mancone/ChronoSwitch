// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StreamLevelLoader.generated.h"

class UBoxComponent;

/**
 * A persistent actor that handles streaming levels in/out based on player overlap.
 */
UCLASS()
class CHRONOSWITCH_API AStreamLevelLoader : public AActor
{
	GENERATED_BODY()
	
public:	
	AStreamLevelLoader();

	// The level currently loaded (to be unloaded)
	UPROPERTY(EditInstanceOnly, Category = "Level Streaming")
	TSoftObjectPtr<UWorld> CurrentLevel;

	// The level to load next
	UPROPERTY(EditInstanceOnly, Category = "Level Streaming")
	TSoftObjectPtr<UWorld> NextLevel;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerZone;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Streaming|Timing")
	float MinTransitionDuration = 5.0f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Level Streaming|Events")
	void ReceiveTransitionStart();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Level Streaming|Events")
	void ReceiveTransitionEnd();
	
	// RPC to trigger the start visuals on all clients
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TransitionStart();

	// RPC to trigger the end visuals on all clients
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TransitionEnd();

	// Unload current level
	void StartLevelTransition();

	// Load next level 
	UFUNCTION()
	void OnLevelUnloaded();

	// Finalize 
	UFUNCTION()
	void OnLevelLoaded();
	
	UFUNCTION()
	void OnWaitTimerFinished();
	
	void CheckTransitionComplete();

	FLatentActionInfo GetLatentAction(int32 ID, FName FunctionName);
	
private:
	UPROPERTY()
	TArray<TObjectPtr<APawn>> PlayersInZone;
	
	bool bIsTransitioning = false;
	bool bIsNextLevelReady = false;
	bool bIsWaitTimerDone = false;
	
	FTimerHandle WaitTimerHandle;
};