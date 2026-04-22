// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MainMenuController.h"

#include "Blueprint/UserWidget.h"
#include "Game/ChronoSwitchGameInstance.h"
#include "UI/InviteReceivedWidget.h"
#include "UI/LevelSelectionWidget.h"
#include "UI/SessionsWidget.h"

void AMainMenuController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("MainMenuController.cpp: BeginPlay"));
	
	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());
	
	SessionsWidget = CreateWidget<USessionsWidget>(GetWorld(), SessionsWidgetClass);
	LevelSelectionWidget = CreateWidget<ULevelSelectionWidget>(GetWorld(), LevelSelectionWidgetClass);
	InviteReceivedWidget = CreateWidget<UInviteReceivedWidget>(GetWorld(), InviteReceivedWidgetClass);
	
	if (SessionsWidget &&  LevelSelectionWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("MainMenuController.cpp: Added Sessions Widget to viewport"));
		SessionsWidget->AddToViewport();
	}
	
	Cast<UChronoSwitchGameInstance>(GetGameInstance())->OnInviteReceivedSignal.AddDynamic(this, &AMainMenuController::OnInviteReceived);
}

void AMainMenuController::ChangeWidget()
{
	SessionsWidget->RemoveFromParent();
	UE_LOG(LogTemp, Warning, TEXT("MainMenuController.cpp: Added Level Selection Widget to viewport"));
	LevelSelectionWidget->AddToViewport();
}

void AMainMenuController::OnInviteReceived()
{
	if (InviteReceivedWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("MainMenuController.cpp: Added Invite Received Widget to viewport"));
		InviteReceivedWidget->AddToViewport();
	}
}
