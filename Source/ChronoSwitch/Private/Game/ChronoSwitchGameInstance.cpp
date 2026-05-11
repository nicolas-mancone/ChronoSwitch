// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/ChronoSwitchGameInstance.h"

#include "Game/SessionSubsystem.h"
#include "GameFramework/PlayerController.h"

UChronoSwitchGameInstance::UChronoSwitchGameInstance()
{
}

void UChronoSwitchGameInstance::Init()
{
	Super::Init();

	if (USessionSubsystem* SessionSubsystem = GetSubsystem<USessionSubsystem>())
	{
		SessionSubsystem->OnInviteReceivedSignal.AddDynamic(this, &UChronoSwitchGameInstance::HandleInviteReceived);
		SessionSubsystem->OnCreateSessionFinishedSignal.AddDynamic(this, &UChronoSwitchGameInstance::HandleCreateSessionFinished);
		SessionSubsystem->OnJoinSessionFinishedSignal.AddDynamic(this, &UChronoSwitchGameInstance::HandleJoinSessionFinished);
		SessionSubsystem->OnLeaveSessionFinishedSignal.AddDynamic(this, &UChronoSwitchGameInstance::HandleLeaveSessionFinished);
	}
}

void UChronoSwitchGameInstance::Shutdown()
{
	if (USessionSubsystem* SessionSubsystem = GetSubsystem<USessionSubsystem>())
	{
		SessionSubsystem->OnInviteReceivedSignal.RemoveDynamic(this, &UChronoSwitchGameInstance::HandleInviteReceived);
		SessionSubsystem->OnCreateSessionFinishedSignal.RemoveDynamic(this, &UChronoSwitchGameInstance::HandleCreateSessionFinished);
		SessionSubsystem->OnJoinSessionFinishedSignal.RemoveDynamic(this, &UChronoSwitchGameInstance::HandleJoinSessionFinished);
		SessionSubsystem->OnLeaveSessionFinishedSignal.RemoveDynamic(this, &UChronoSwitchGameInstance::HandleLeaveSessionFinished);
	}

	Super::Shutdown();
}

void UChronoSwitchGameInstance::HandleInviteReceived()
{
	OnInviteReceivedSignal.Broadcast();
}

void UChronoSwitchGameInstance::HandleCreateSessionFinished(bool bWasSuccessful, FString ErrorMessage)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Create session failed: %s"), *ErrorMessage);
		return;
	}

	TravelToLobby();
}

void UChronoSwitchGameInstance::HandleJoinSessionFinished(bool bWasSuccessful, FString ConnectStringOrErrorMessage)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Join session failed: %s"), *ConnectStringOrErrorMessage);
		return;
	}

	if (ConnectStringOrErrorMessage.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Join session succeeded but connect string is empty"));
		return;
	}

	if (APlayerController* PC = GetFirstLocalPlayerController())
	{
		UE_LOG(LogTemp, Warning, TEXT("ClientTravel to joined session: %s"), *ConnectStringOrErrorMessage);
		PC->ClientTravel(ConnectStringOrErrorMessage, ETravelType::TRAVEL_Absolute);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot travel to joined session: local PlayerController not found"));
	}
}

void UChronoSwitchGameInstance::HandleLeaveSessionFinished(bool bWasSuccessful, FString ErrorMessage)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Leave session failed: %s"), *ErrorMessage);
		return;
	}

	TravelToMainMenu();
}

void UChronoSwitchGameInstance::TravelToLobby()
{
	if (LobbyMap.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyMap is not set in GameInstance"));
		return;
	}

	const FString TravelURL = FString::Printf(TEXT("%s?listen"), *LobbyMap.ToSoftObjectPath().GetLongPackageName());
	UE_LOG(LogTemp, Warning, TEXT("ServerTravel to lobby: %s"), *TravelURL);

	if (!GetWorld() || !GetWorld()->ServerTravel(TravelURL))
	{
		UE_LOG(LogTemp, Error, TEXT("ServerTravel failed for URL: %s"), *TravelURL);
	}
}

void UChronoSwitchGameInstance::TravelToMainMenu()
{
	if (MainMenuMap.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("MainMenuMap is not set in GameInstance"));
		return;
	}

	const FString MainMenuURL = MainMenuMap.ToSoftObjectPath().GetLongPackageName();
	UE_LOG(LogTemp, Warning, TEXT("ClientTravel to main menu: %s"), *MainMenuURL);

	if (APlayerController* PC = GetFirstLocalPlayerController())
	{
		PC->ClientTravel(MainMenuURL, ETravelType::TRAVEL_Absolute);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot travel to main menu: local PlayerController not found"));
	}
}