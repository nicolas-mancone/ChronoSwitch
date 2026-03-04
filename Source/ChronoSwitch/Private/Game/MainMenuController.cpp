// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MainMenuController.h"

#include "Blueprint/UserWidget.h"
#include "Game/ChronoSwitchGameInstance.h"
#include "UI/InviteReceivedWidget.h"
#include "UI/SessionsWidget.h"

void AMainMenuController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("MainMenuController.cpp: BeginPlay"));
	
	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());
	
	SessionsWidget = CreateWidget<USessionsWidget>(this, SessionsWidgetClass);
	InviteReceivedWidget = CreateWidget<UInviteReceivedWidget>(this, InviteReceivedWidgetClass);
	
	if (SessionsWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("MainMenuController.cpp: Added widget to viewport"));
		SessionsWidget->AddToViewport();
	}
	
	Cast<UChronoSwitchGameInstance>(GetGameInstance())->OnInviteReceivedSignal.AddDynamic(this, &AMainMenuController::OnInviteReceived);
}

void AMainMenuController::OnInviteReceived()
{
	InviteReceivedWidget->AddToViewport();
}
