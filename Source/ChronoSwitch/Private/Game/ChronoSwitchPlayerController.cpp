// Fill out your copyright notice in the Description page of Project Settings.


#include "ChronoSwitch/Public/Game/ChronoSwitchPlayerController.h"

void AChronoSwitchPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	ChangeInputMode(false);
}

void AChronoSwitchPlayerController::ChangeInputMode(bool bIsPaused)
{
	if (!IsLocalController()) // Should be useless
		return;
	bShowMouseCursor = bIsPaused;
	if (bIsPaused)
	{
		SetInputMode(FInputModeUIOnly());
	}
	else
	{
		SetInputMode(FInputModeGameOnly());
	}
}
