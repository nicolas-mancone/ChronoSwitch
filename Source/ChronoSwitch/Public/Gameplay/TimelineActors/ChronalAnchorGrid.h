// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChronalAnchorGrid.generated.h"

class AChronoSwitchCharacter;
class UBoxComponent;
class UArrowComponent;

/**
 * Enum to easly specify in editor the timeline to force onto the players
 */
UENUM(BlueprintType)
enum class EForcedTimeline : uint8
{
	Past UMETA(DisplayName="Past"),
	Future UMETA(DisplayName="Future"),
	None UMETA(DisplayName="None")
	// "None" must be last for casting purposes
};

UENUM(BlueprintType)
enum class ECrossingEffectMode : uint8
{
	Disable UMETA(DisplayName="Disable"),
	Enable UMETA(DisplayName="Enable"),
	None UMETA(DisplayName="None")
	// "None" must be last for casting purposes
};

USTRUCT(BlueprintType)
struct FCrossingSettings
{
	GENERATED_BODY()
	
	// Target Timeline of the C.A.G. Zone
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CAG Settings")
	EForcedTimeline TargetForcedTimeline = EForcedTimeline::Past;
	
	// Dictates whether it should disable visor or not
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CAG Settings")
	ECrossingEffectMode VisorMode = ECrossingEffectMode::None;
	
	// Dictates whether it should disable visor or not
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CAG Settings")
	ECrossingEffectMode SwitchMode = ECrossingEffectMode::None;
};
/**
 * Class for Chronal Anchor Grid portals that lock the player in a specific
 * timeline, and can disable visor
 */
UCLASS()
class CHRONOSWITCH_API AChronalAnchorGrid : public AActor
{
	GENERATED_BODY()
	//To Do: Use case of objects that passes the barrier?
	
public:
#pragma region Lifecycle
	// Sets default values for this actor's properties
	AChronalAnchorGrid();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma endregion
	
#pragma region Settings
	
	// The struct stores setting for entering the barrier
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FCrossingSettings EnteringCrossingSettings;
	
	// The struct stores setting for exiting the barrier
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FCrossingSettings ExitingCrossingSettings;
	
	// The map stores the surface normal at the entry point for each player
	UPROPERTY()
	TMap<AChronoSwitchCharacter*, FVector> StoredEntryNormals;
	
#pragma endregion
	
protected:
#pragma region Lifecycle
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
#pragma endregion
	
#pragma region Components
	/** Mesh of the Chronal Anchor Grid barrier */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BarrierMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* GridBorder1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* GridBorder2;
	
	// Entering the barrier in the direction of the EntranceDirection arrow activates the C.A.G.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UArrowComponent* EntranceDirectionArrow;
	
#pragma endregion
	
#pragma region Collision Delegates
	UFUNCTION(BlueprintCallable, Category = "Collision")
	void OnBeginOverlap(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& Hit);
	
	UFUNCTION(BlueprintCallable, Category = "Collision")
	void OnEndOverlap(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex);

#pragma endregion

	UFUNCTION(BlueprintImplementableEvent, Category = "Sound")
	void ManageSoundOnCrossing();
	
private:
	
#pragma region Utility
	/** Calculates the surface normal of the barrier closest to the actor */
	UFUNCTION()
	FVector GetInteractionNormal(const AActor* Actor) const;
	
#pragma endregion
};
