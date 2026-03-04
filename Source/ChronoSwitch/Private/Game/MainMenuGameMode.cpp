// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MainMenuGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Game/ChronoSwitchGameInstance.h"
#include "UI/InviteReceivedWidget.h"
#include "UI/SessionsWidget.h"


AMainMenuGameMode::AMainMenuGameMode()
{
}

void AMainMenuGameMode::StartPlay()
{
	Super::StartPlay();
	/*
	SessionsWidget = CreateWidget<USessionsWidget>(GetWorld(), USessionsWidget::StaticClass());
	InviteReceivedWidget = CreateWidget<UInviteReceivedWidget>(GetWorld(), UInviteReceivedWidget::StaticClass());
	SessionsWidget->AddToViewport();
	*/
	 
	//Cast<UChronoSwitchGameInstance>(GetGameInstance())->OnInviteReceivedSignal.AddDynamic(this, &AMainMenuGameMode::OnInviteReceived);
}
/*
void AMainMenuGameMode::OnInviteReceived()
{
	//InviteReceivedWidget->AddToViewport();
}
*/