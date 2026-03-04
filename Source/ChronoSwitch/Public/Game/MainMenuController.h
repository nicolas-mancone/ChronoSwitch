// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuController.generated.h"

class USessionsWidget;
class UInviteReceivedWidget;

/**
 * 
 */
UCLASS()
class CHRONOSWITCH_API AMainMenuController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> SessionsWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> InviteReceivedWidgetClass;
	
	UPROPERTY()
	USessionsWidget* SessionsWidget;
	UPROPERTY()
	UInviteReceivedWidget* InviteReceivedWidget;
	
protected:
	UFUNCTION()
	void OnInviteReceived();
};
