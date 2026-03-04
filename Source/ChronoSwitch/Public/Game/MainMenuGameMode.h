// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

class USessionsWidget;
class UInviteReceivedWidget;

/**
 * 
 */
UCLASS()
class CHRONOSWITCH_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMainMenuGameMode();
	
	virtual void StartPlay() override;
	
protected:
	UPROPERTY()
	USessionsWidget* SessionsWidget;
	
	UPROPERTY()
	UInviteReceivedWidget* InviteReceivedWidget;
	
	UFUNCTION()
	void OnInviteReceived();
};
