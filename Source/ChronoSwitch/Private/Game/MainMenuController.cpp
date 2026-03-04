// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MainMenuController.h"

void AMainMenuController::BeginPlay()
{
	Super::BeginPlay();
	
	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());
}
