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

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Unload current level
	void StartLevelTransition();

	// Load next level 
	UFUNCTION()
	void OnLevelUnloaded();

	// Finalize 
	UFUNCTION()
	void OnLevelLoaded();

	FLatentActionInfo GetLatentAction(int32 ID, FName FunctionName);
};