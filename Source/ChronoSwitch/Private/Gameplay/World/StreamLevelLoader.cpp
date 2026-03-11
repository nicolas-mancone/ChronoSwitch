// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/World/StreamLevelLoader.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AStreamLevelLoader::AStreamLevelLoader()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));
	RootComponent = TriggerZone;
	TriggerZone->SetCollisionProfileName(TEXT("Trigger"));
}

void AStreamLevelLoader::BeginPlay()
{
	Super::BeginPlay();
	TriggerZone->OnComponentBeginOverlap.AddDynamic(this, &AStreamLevelLoader::OnOverlapBegin);
}

void AStreamLevelLoader::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass()) && HasAuthority())
	{
		// TODO: Check if ALL players are inside before starting.
		StartLevelTransition();
	}
}

void AStreamLevelLoader::StartLevelTransition()
{
	UE_LOG(LogTemp, Warning, TEXT("Loader: Transition Starting..."));

	// Unload Current Level
	if (!CurrentLevel.IsNull())
	{
		FName LevelName = FName(*CurrentLevel.ToSoftObjectPath().GetLongPackageName());
		FLatentActionInfo LatentInfo = GetLatentAction(0, "OnLevelUnloaded");
		UGameplayStatics::UnloadStreamLevel(this, LevelName, LatentInfo, false);
	}
	else
	{
		// No level to unload, go straight to loading
		OnLevelUnloaded();
	}
}

void AStreamLevelLoader::OnLevelUnloaded()
{
	UE_LOG(LogTemp, Warning, TEXT("Loader: Level Unloaded. Loading Next..."));

	// Load Next Level
	if (!NextLevel.IsNull())
	{
		FName LevelName = FName(*NextLevel.ToSoftObjectPath().GetLongPackageName());
		FLatentActionInfo LatentInfo = GetLatentAction(1, "OnLevelLoaded");
		UGameplayStatics::LoadStreamLevel(this, LevelName, true, true, LatentInfo);
	}
}

void AStreamLevelLoader::OnLevelLoaded()
{
	UE_LOG(LogTemp, Warning, TEXT("Loader: Next Level Ready."));
	
	// Update References 
	CurrentLevel = NextLevel;
	NextLevel = nullptr; 
}

FLatentActionInfo AStreamLevelLoader::GetLatentAction(int32 ID, FName FunctionName)
{
	FLatentActionInfo Info;
	Info.CallbackTarget = this;
	Info.ExecutionFunction = FunctionName;
	Info.Linkage = ID;
	Info.UUID = ID; // Unique ID for the latent action manager
	return Info;
}