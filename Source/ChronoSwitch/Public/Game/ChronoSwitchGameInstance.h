// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ChronoSwitchGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInviteReceivedSignal);

/**
 * Game instance responsible only for project-specific session flow, such as map travel.
 * Online session logic is handled by USessionSubsystem.
 */
UCLASS()
class CHRONOSWITCH_API UChronoSwitchGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UChronoSwitchGameInstance();

	virtual void Init() override;
	virtual void Shutdown() override;

	/** Map opened by the host after a session is created successfully. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Session|Maps")
	TSoftObjectPtr<UWorld> LobbyMap;

	/** Map opened when leaving a session. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Session|Maps")
	TSoftObjectPtr<UWorld> MainMenuMap;

	UPROPERTY(BlueprintAssignable, Category = "Session|Events")
	FOnInviteReceivedSignal OnInviteReceivedSignal;

private:
	UFUNCTION()
	void HandleInviteReceived();

	UFUNCTION()
	void HandleCreateSessionFinished(bool bWasSuccessful, FString ErrorMessage);

	UFUNCTION()
	void HandleJoinSessionFinished(bool bWasSuccessful, FString ConnectStringOrErrorMessage);

	UFUNCTION()
	void HandleLeaveSessionFinished(bool bWasSuccessful, FString ErrorMessage);

	void TravelToLobby();
	void TravelToMainMenu();
};