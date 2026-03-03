// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ChronoSwitchGameInstance.generated.h"

// Search Settings Key Defines
#define SEARCH_PRESENCE FName(TEXT("PRESENCESEARCH"))

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
	
	UFUNCTION(BlueprintCallable)
	void HostSession(int32 MaxPlayers = 2);

	UFUNCTION(BlueprintCallable)
	void FindJoinSession();
	
protected:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
};
