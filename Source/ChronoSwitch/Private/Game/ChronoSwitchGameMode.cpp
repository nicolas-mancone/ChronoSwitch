// Fill out your copyright notice in the Description page of Project Settings.


#include "ChronoSwitch/Public/Game/ChronoSwitchGameMode.h"

#include "Game/ChronoSwitchGameState.h"


AChronoSwitchGameMode::AChronoSwitchGameMode()
{
	bUseSeamlessTravel = true;
}

void AChronoSwitchGameMode::StartPlay()
{
	Super::StartPlay();

	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("GameMode avviato correttamente"));
	}
	
	UE_LOG(LogTemp, Log, TEXT("AChronoSwitchGameMode::StartPlay chiamato con successo"));

	// if (const UNetDriver* NetDriver = GetWorld()->GetNetDriver())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("NetDriver is in use: %s"), *NetDriver->GetClass()->GetName());
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("No NetDriver found!"));
	// }
}

void AChronoSwitchGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
}
