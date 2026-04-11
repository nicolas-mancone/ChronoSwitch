// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/World/StreamLevelLoader.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AStreamLevelLoader::AStreamLevelLoader()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;

	TriggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));
	RootComponent = TriggerZone;
	TriggerZone->SetCollisionProfileName(TEXT("Trigger"));
}

void AStreamLevelLoader::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		TriggerZone->OnComponentBeginOverlap.AddDynamic(this, &AStreamLevelLoader::OnOverlapBegin);
		TriggerZone->OnComponentEndOverlap.AddDynamic(this, &AStreamLevelLoader::OnOverlapEnd);
	}
}

void AStreamLevelLoader::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if (PlayerPawn && PlayerPawn->IsPlayerControlled() && !bIsTransitioning)
	{
		PlayersInZone.AddUnique(PlayerPawn);
 
		// Check if all players in the session are inside the lift
		const int32 NumPlayers = GetWorld()->GetNumPlayerControllers();
		if (NumPlayers > 0 && PlayersInZone.Num() >= NumPlayers)
		{
			StartLevelTransition();
		}
	}
}

void AStreamLevelLoader::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if (PlayerPawn && !bIsTransitioning)
	{
		PlayersInZone.Remove(PlayerPawn);
	}
}

void AStreamLevelLoader::StartLevelTransition()
{
	UE_LOG(LogTemp, Warning, TEXT("Loader: Transition Starting..."));

	bIsTransitioning = true;
	bIsNextLevelReady = false;
	bIsWaitTimerDone = false;
 
	// Tell ALL clients (and the server) to close doors and start Niagara effects
	Multicast_TransitionStart();
 
	// Start the minimum wait timer
	GetWorld()->GetTimerManager().SetTimer(WaitTimerHandle, this, &AStreamLevelLoader::OnWaitTimerFinished, MinTransitionDuration, false);
	
	// Delay the unload to allow doors to close visually
	if (UnloadDelay > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(UnloadTimerHandle, this, &AStreamLevelLoader::ExecuteUnload, UnloadDelay, false);
	}
	else
	{
		ExecuteUnload();
	}
}

void AStreamLevelLoader::ExecuteUnload()
{
	// Execute Unload Current Level
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
		UGameplayStatics::LoadStreamLevel(this, LevelName, true, false, LatentInfo);
	}
}

void AStreamLevelLoader::OnLevelLoaded()
{
	UE_LOG(LogTemp, Warning, TEXT("Loader: Next Level Ready."));
	
	bIsNextLevelReady = true;
	CheckTransitionComplete();
}

void AStreamLevelLoader::OnWaitTimerFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("Loader: Wait Timer Finished."));
 
	bIsWaitTimerDone = true;
	CheckTransitionComplete();
}

void AStreamLevelLoader::CheckTransitionComplete()
{
	// Only proceed if BOTH the loading is done AND the minimum visual time has passed
	if (bIsNextLevelReady && bIsWaitTimerDone)
	{
		UE_LOG(LogTemp, Warning, TEXT("Loader: Transition Fully Complete. Opening Doors."));
 
		// Update References for the next transition
		CurrentLevel = NextLevel;
		NextLevel = nullptr; 
 
		// Tell ALL clients (and the server) to stop effects and open doors
		Multicast_TransitionEnd();
		
	}
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

void AStreamLevelLoader::Multicast_TransitionStart_Implementation()
{
	ReceiveTransitionStart();
}

void AStreamLevelLoader::Multicast_TransitionEnd_Implementation()
{
	ReceiveTransitionEnd();
}
