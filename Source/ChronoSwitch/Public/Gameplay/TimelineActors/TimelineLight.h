#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimelineLight.generated.h"

class USceneComponent;
class ULightComponent;
class UTimelineObserverComponent;

UENUM(BlueprintType)
enum class ETimelineLightMode : uint8
{
	PastOnly        UMETA(DisplayName = "Past Only"),
	FutureOnly      UMETA(DisplayName = "Future Only"),
	ColorShift      UMETA(DisplayName = "Both (Color/Intensity Shift)")
};

/**
 * A highly optimized light actor that locally reacts to the player's timeline state.
 */
UCLASS()
class CHRONOSWITCH_API ATimelineLight : public AActor
{
	GENERATED_BODY()
	
public:	
	ATimelineLight();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	/** Root component to allow attaching various light types (Point, Spot, Rect) via Blueprint. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTimelineObserverComponent> TimelineObserver;

	/** Dynamically resolved light component attached to this actor. */
	UPROPERTY(BlueprintReadOnly, Category = "Components")
	TObjectPtr<ULightComponent> LightComponent;

	// --- Light Configuration ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Light")
	ETimelineLightMode LightMode = ETimelineLightMode::ColorShift;
	
	/** The units to use for the light's intensity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Light")
	ELightUnits IntensityUnits = ELightUnits::Candelas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Light|Past State")
	FLinearColor PastColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Light|Past State")
	float PastIntensity = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Light|Future State")
	FLinearColor FutureColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Light|Future State")
	float FutureIntensity = 5000.0f;

	// --- Temperature Settings ---

	/** If true, the light will use Temperature (in Kelvin) instead of the RGB color. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Light|Temperature")
	bool bUseTemperature = false;

	/** Light temperature (in Kelvin) for the Past timeline. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Light|Temperature", meta = (EditCondition = "bUseTemperature", UIMin = "1700", UIMax = "12000"))
	float PastTemperature = 8000.0f;

	/** Light temperature (in Kelvin) for the Future timeline. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Light|Temperature", meta = (EditCondition = "bUseTemperature", UIMin = "1700", UIMax = "12000"))
	float FutureTemperature = 4000.0f;

	// --- Transition Settings ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Light|Transition")
	bool bSmoothTransition = true;

	/** Interpolation speed for color and intensity changes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Light|Transition", meta = (EditCondition = "bSmoothTransition"))
	float TransitionSpeed = 10.0f;

	UFUNCTION()
	void HandleTimelineUpdated(uint8 CurrentTimelineID, bool bIsVisorActive);

private:
	FLinearColor CurrentColor;
	float CurrentIntensity;
	float CurrentTemperature;
	FLinearColor TargetColor;
	float TargetIntensity;
	float TargetTemperature;
	bool bIsTurningOff = false;
	bool bHasInitialized = false;
};