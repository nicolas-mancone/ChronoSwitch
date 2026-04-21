// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PlayerVisorWidget.generated.h"

/**
 * 
 */
UCLASS()
class CHRONOSWITCH_API UPlayerVisorWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetPromptText(const FText& Text);
	
	UFUNCTION(BlueprintCallable)
	void SetPromptVisibility(ESlateVisibility NewVisibility);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PromptTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	UImage* SwitchModeImage;
	
	UPROPERTY(meta = (BindWidget))
	UImage* FriendModeImage;
};
