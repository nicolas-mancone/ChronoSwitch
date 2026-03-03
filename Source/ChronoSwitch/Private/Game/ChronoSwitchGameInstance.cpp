// Fill out your copyright notice in the Description page of Project Settings.


#include "ChronoSwitch/Public/Game/ChronoSwitchGameInstance.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"

// To Do: Test if GetWorld() can be reliably used in GameInstance

UChronoSwitchGameInstance::UChronoSwitchGameInstance() { }

void UChronoSwitchGameInstance::Init()
{
	Super::Init();
	
	if (IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Subsystem: %s"), *OnlineSubsystem->GetSubsystemName().ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Subsystem Not Found"));
	}
}

#pragma region Host Join Logic

void UChronoSwitchGameInstance::HostSession(int32 MaxPlayers)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr SessionInterface = Subsystem ? Subsystem->GetSessionInterface() : nullptr;
	
	if (!SessionInterface.IsValid()) return;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Host Joining Logic"));
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = false;
	SessionSettings.NumPublicConnections = MaxPlayers;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	// This functionalities should help with using steam functionalities such as Friends Invites

	// Store the handle to clear it later
	SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UChronoSwitchGameInstance::OnCreateSessionComplete));
	
	SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);
}

void UChronoSwitchGameInstance::FindJoinSession()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr SessionInterface = Subsystem ? Subsystem->GetSessionInterface() : nullptr;
	
	if (!SessionInterface.IsValid()) return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10; // We should need less
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UChronoSwitchGameInstance::OnFindSessionsComplete));
	
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

#pragma endregion

#pragma region Delegates

void UChronoSwitchGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr SessionInterface = Subsystem ? Subsystem->GetSessionInterface() : nullptr;
	
	// Cleanup the delegate
	SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("OnSessionCreated"));
	if (bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("OnSessionCreatedSuccess"));
		if (!LobbyMap.IsNull())
		{
			FString TravelURL = FString::Printf(TEXT("%s?listen"), *LobbyMap.ToSoftObjectPath().GetLongPackageName());
			GetWorld()->ServerTravel(TravelURL);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("LobbyMap is not set in GameInstance!"));
		}
	}
}

void UChronoSwitchGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr SessionInterface = Subsystem ? Subsystem->GetSessionInterface() : nullptr;
	
	SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);

	if (bWasSuccessful && SessionSearch->SearchResults.Num() > 0)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UChronoSwitchGameInstance::OnJoinSessionComplete));
		
		//Need to use specific Session name
		SessionInterface->JoinSession(0, NAME_GameSession, SessionSearch->SearchResults[0]);
	}
}

void UChronoSwitchGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineSessionPtr SessionInterface = Subsystem ? Subsystem->GetSessionInterface() : nullptr;
	
	SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		if (APlayerController* PC = GetFirstLocalPlayerController())
		{
			FString JoinURL;
			if (SessionInterface->GetResolvedConnectString(SessionName, JoinURL))
			{
				PC->ClientTravel(JoinURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

#pragma endregion