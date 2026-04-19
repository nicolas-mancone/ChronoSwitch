// Fill out your copyright notice in the Description page of Project Settings.


#include "ChronoSwitch/Public/Game/ChronoSwitchGameInstance.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

UChronoSwitchGameInstance::UChronoSwitchGameInstance() { }

IOnlineSessionPtr UChronoSwitchGameInstance::GetSessionInterface() const
{
	if (IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld()))
	{
		return OnlineSubsystem->GetSessionInterface();
	}
	return nullptr;
}

void UChronoSwitchGameInstance::Init()
{
	Super::Init();
	
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Subsystem or Session Interface not found"));
		return;
	}
	
	InviteAcceptedDelegateHandle = SessionInterface->AddOnSessionUserInviteAcceptedDelegate_Handle(
		FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UChronoSwitchGameInstance::OnInviteAccepted));

	InviteReceivedDelegateHandle = SessionInterface->AddOnSessionInviteReceivedDelegate_Handle(
		FOnSessionInviteReceivedDelegate::CreateUObject(this, &UChronoSwitchGameInstance::OnInviteReceived));
}

void UChronoSwitchGameInstance::Shutdown()
{
	if (IOnlineSessionPtr SessionInterface = GetSessionInterface(); SessionInterface.IsValid())
	{
		SessionInterface->ClearOnSessionUserInviteAcceptedDelegate_Handle(InviteAcceptedDelegateHandle);
		SessionInterface->ClearOnSessionInviteReceivedDelegate_Handle(InviteReceivedDelegateHandle);
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	Super::Shutdown();
}

#pragma region Host Join Logic

void UChronoSwitchGameInstance::HostSession(int32 MaxPlayers)
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("HostSession failed: SessionInterface invalid"));
		return;
	}

	// If a session already exists, destroy it first.
	if (SessionInterface->GetNamedSession(NAME_GameSession) != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Existing session found, destroying it before creating a new one"));

		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
			FOnDestroySessionCompleteDelegate::CreateUObject(this, &UChronoSwitchGameInstance::OnDestroySessionComplete));

		SessionInterface->DestroySession(NAME_GameSession);
		return;
	}

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = false;
	SessionSettings.NumPublicConnections = MaxPlayers;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.BuildUniqueId = 1;

	SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateUObject(this, &UChronoSwitchGameInstance::OnCreateSessionComplete));

	if (!SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings))
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateSession returned false"));
	}
}

void UChronoSwitchGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	UE_LOG(LogTemp, Warning, TEXT("OnDestroySessionComplete: %s, success=%s"), *SessionName.ToString(), bWasSuccessful ? TEXT("true") : TEXT("false"));

	if (!bWasSuccessful)
	{
		return;
	}

	HostSession();
}

void UChronoSwitchGameInstance::FindJoinSession()
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("FindJoinSession failed: SessionInterface invalid"));
		return;
	}

	SessionSearch = MakeShared<FOnlineSessionSearch>();
	SessionSearch->MaxSearchResults = 10;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(this, &UChronoSwitchGameInstance::OnFindSessionsComplete));

	if (!SessionInterface->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		UE_LOG(LogTemp, Warning, TEXT("FindSessions returned false"));
	}
}

#pragma endregion

#pragma region ExternalUtilities

void UChronoSwitchGameInstance::OpenExternalInviteDialog()
{
	if (IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld()))
	{
		IOnlineExternalUIPtr ExternalUI = Subsystem->GetExternalUIInterface();
		if (ExternalUI.IsValid())
		{
			ExternalUI->ShowInviteUI(0, NAME_GameSession);
		}
	}
}

#pragma endregion

#pragma region Delegates

void UChronoSwitchGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete: %s, success=%s"), *SessionName.ToString(), bWasSuccessful ? TEXT("true") : TEXT("false"));

	if (!bWasSuccessful)
	{
		return;
	}

	if (LobbyMap.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyMap is not set in GameInstance!"));
		return;
	}

	const FString TravelURL = FString::Printf(TEXT("%s?listen"), *LobbyMap.ToSoftObjectPath().GetLongPackageName());
	UE_LOG(LogTemp, Warning, TEXT("ServerTravel to: %s"), *TravelURL);

	if (!GetWorld() || !GetWorld()->ServerTravel(TravelURL))
	{
		UE_LOG(LogTemp, Error, TEXT("ServerTravel failed for URL: %s"), *TravelURL);
		return;
	}

	OpenExternalInviteDialog();
}

void UChronoSwitchGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	if (!bWasSuccessful || !SessionSearch.IsValid() || SessionSearch->SearchResults.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("FindSessions failed or no results found"));
		return;
	}

	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
			FOnJoinSessionCompleteDelegate::CreateUObject(this, &UChronoSwitchGameInstance::OnJoinSessionComplete));

		if (!SessionInterface->JoinSession(0, NAME_GameSession, SessionSearch->SearchResults[0]))
		{
			UE_LOG(LogTemp, Warning, TEXT("JoinSession returned false"));
		}
	}
}

void UChronoSwitchGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}
	
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		if (APlayerController* PC = GetFirstLocalPlayerController())
		{
			FString JoinURL;
			if (SessionInterface.IsValid() && SessionInterface->GetResolvedConnectString(SessionName, JoinURL))
			{
				PC->ClientTravel(JoinURL, ETravelType::TRAVEL_Absolute);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to resolve connect string for session: %s"), *SessionName.ToString());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("JoinSession failed with result: %d"), static_cast<int32>(Result));
	}
}

void UChronoSwitchGameInstance::OnInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum, TSharedPtr<const FUniqueNetId> PersonInviting, const FOnlineSessionSearchResult& InviteResult)
{
	IOnlineSessionPtr SessionInterface = GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		return;
	}

	SessionInterface->ClearOnSessionUserInviteAcceptedDelegate_Handle(InviteAcceptedDelegateHandle);

	if (!bWasSuccessful || !InviteResult.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Invite accepted callback failed or invite is invalid"));
		return;
	}

	SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
		FOnJoinSessionCompleteDelegate::CreateUObject(this, &UChronoSwitchGameInstance::OnJoinSessionComplete));

	if (!SessionInterface->JoinSession(LocalUserNum, NAME_GameSession, InviteResult))
	{
		UE_LOG(LogTemp, Warning, TEXT("JoinSession from invite returned false"));
	}
}

void UChronoSwitchGameInstance::OnInviteReceived(const FUniqueNetId& UserId, const FUniqueNetId& FromId,
	const FString& AppId, const FOnlineSessionSearchResult& InviteResult)
{
	OnInviteReceivedSignal.Broadcast();
}

#pragma endregion
