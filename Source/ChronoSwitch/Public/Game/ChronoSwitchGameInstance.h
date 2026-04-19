// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ChronoSwitchGameInstance.generated.h"

// Search Settings Key Defines
#define SEARCH_PRESENCE FName(TEXT("PRESENCESEARCH"))

// Multicast delegate signatures
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInviteReceivedSignal);

/**
 * 
 */
UCLASS()
class CHRONOSWITCH_API UChronoSwitchGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UChronoSwitchGameInstance();
	
	virtual void Init() override;
	virtual void Shutdown() override;
	
	UFUNCTION(BlueprintCallable)
	void HostSession(int32 MaxPlayers = 2);

	UFUNCTION(BlueprintCallable)
	void FindJoinSession();
	
	UFUNCTION(BlueprintCallable)
	void OpenExternalInviteDialog();
	
	/** The map to travel to when hosting a session. Format: "/Game/Path/To/Map" */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Session")
	TSoftObjectPtr<UWorld> LobbyMap;
	
	// Broadcast Signals
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInviteReceivedSignal OnInviteReceivedSignal;
	
protected:
	
	IOnlineSessionPtr GetSessionInterface() const;
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum, TSharedPtr<const FUniqueNetId> PersonInviting, const FOnlineSessionSearchResult& InviteResult);
	void OnInviteReceived(const FUniqueNetId& UserId, const FUniqueNetId& FromId, const FString& AppId, const FOnlineSessionSearchResult& InviteResult);
	
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
private:
	// Session Interface Handles
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	FDelegateHandle InviteAcceptedDelegateHandle;
	FDelegateHandle InviteReceivedDelegateHandle;
};
