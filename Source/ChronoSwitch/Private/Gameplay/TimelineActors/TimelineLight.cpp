#include "Gameplay/TimelineActors/TimelineLight.h"
#include "Components/LightComponent.h"
#include "Gameplay/ActorComponents/TimelineObserverComponent.h"

ATimelineLight::ATimelineLight()
{
	// Tick is dynamically enabled only during transitions to save CPU cycles.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	bReplicates = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	TimelineObserver = CreateDefaultSubobject<UTimelineObserverComponent>(TEXT("TimelineObserver"));
}

void ATimelineLight::BeginPlay()
{
	Super::BeginPlay();

	LightComponent = FindComponentByClass<ULightComponent>();
	
	if (LightComponent)
	{
		LightComponent->SetMobility(EComponentMobility::Movable);
	}

	if (TimelineObserver)
	{
		TimelineObserver->OnPlayerTimelineStateUpdated.AddDynamic(this, &ATimelineLight::HandleTimelineUpdated);
	}
}

void ATimelineLight::HandleTimelineUpdated(uint8 CurrentTimelineID, bool bIsVisorActive)
{
	if (!LightComponent) return;

	const bool bIsInPast = (CurrentTimelineID == 0);

	switch (LightMode)
	{
		case ETimelineLightMode::PastOnly:
			TargetColor = PastColor;
			TargetIntensity = bIsInPast ? PastIntensity : 0.0f;
			bIsTurningOff = !bIsInPast;
			break;
			
		case ETimelineLightMode::FutureOnly:
			TargetColor = FutureColor;
			TargetIntensity = !bIsInPast ? FutureIntensity : 0.0f;
			bIsTurningOff = bIsInPast;
			break;
			
		case ETimelineLightMode::ColorShift:
			TargetColor = bIsInPast ? PastColor : FutureColor;
			TargetIntensity = bIsInPast ? PastIntensity : FutureIntensity;
			bIsTurningOff = false;
			break;
	}

	// Bypass transition if disabled or during initial setup to prevent unwanted fade-in effects on level load.
	if (!bSmoothTransition || !bHasInitialized)
	{
		CurrentColor = TargetColor;
		CurrentIntensity = TargetIntensity;

		LightComponent->SetLightColor(TargetColor);
		LightComponent->SetIntensity(TargetIntensity);
		LightComponent->SetVisibility(!bIsTurningOff);
		
		bHasInitialized = true;
		SetActorTickEnabled(false);
		return;
	}

	// Ensure the component is active before fading in.
	if (!bIsTurningOff && !LightComponent->IsVisible())
	{
		LightComponent->SetVisibility(true);
	}

	SetActorTickEnabled(true);
}

void ATimelineLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!LightComponent) return;

	// Use high-precision internal variables for interpolation.
	// Component getters often return quantized 8-bit values (e.g., FColor) which degrade FInterpTo smoothness.
	CurrentIntensity = FMath::FInterpTo(CurrentIntensity, TargetIntensity, DeltaTime, TransitionSpeed);
	CurrentColor = FMath::CInterpTo(CurrentColor, TargetColor, DeltaTime, TransitionSpeed);

	LightComponent->SetIntensity(CurrentIntensity);
	LightComponent->SetLightColor(CurrentColor);

	if (FMath::IsNearlyEqual(CurrentIntensity, TargetIntensity, 1.0f) && CurrentColor.Equals(TargetColor, 0.01f))
	{
		CurrentIntensity = TargetIntensity;
		CurrentColor = TargetColor;
		
		LightComponent->SetIntensity(TargetIntensity);
		LightComponent->SetLightColor(TargetColor);
		
		if (bIsTurningOff) LightComponent->SetVisibility(false);

		// Disable tick once the transition completes to conserve performance.
		SetActorTickEnabled(false);
	}
}