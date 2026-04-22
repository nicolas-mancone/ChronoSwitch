// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/ChronoSwitchCharacter.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "PlayerVisorWidget.generated.h"

UENUM(BlueprintType)
enum class EWidgetSwitchMode : uint8
{
	Disabled,
	Self,
	Friend
};

USTRUCT(BlueprintType)
struct FWidgetImageVariants
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UTexture2D* PastImage;

	UPROPERTY(EditAnywhere)
	UTexture2D* FutureImage;
};

USTRUCT(BlueprintType)
struct FStateInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EWidgetSwitchMode Mode = EWidgetSwitchMode::Disabled;

	UPROPERTY(EditAnywhere)
	bool bCanSwitch = true;
	
	UPROPERTY(EditAnywhere)
	uint8 PlayerTimeline = 0;
	
	UPROPERTY(EditAnywhere)
	uint8 OtherPlayerTimeline = 0;
};

/**
 * 
 */
UCLASS()
class CHRONOSWITCH_API UPlayerVisorWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TMap<EWidgetSwitchMode, FWidgetImageVariants> SwitchModeMap;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FWidgetImageVariants FriendImages;
	
	UPROPERTY(BlueprintReadOnly)
	AChronoSwitchCharacter* OwningCharacter;
	
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu(bool bIsPaused);
	
	UFUNCTION(BlueprintCallable)
	void SetPromptText(const FText& Text);
	
	UFUNCTION(BlueprintCallable)
	void SetPromptVisibility(ESlateVisibility NewVisibility);

	UFUNCTION(BlueprintCallable)
	void UpdateMode(const EWidgetSwitchMode Mode);
	
	UFUNCTION(BlueprintCallable)
	void UpdateCanSwitch(bool bCanSwitch);
	
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerTimeline(const uint8 PlayerTimeline);
	
	UFUNCTION(BlueprintCallable)
	void UpdateOtherPlayerTimeline(const uint8 OtherPlayerTimeline);
	

protected:
	
	UPROPERTY()
	FStateInfo State;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PromptTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	UImage* SwitchModeImage;
	
	UPROPERTY(meta = (BindWidget))
	UImage* FriendModeImage;
	
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* PauseMenuSwitcher;
	
	UFUNCTION(BlueprintCallable)
	void UpdateImages();
};
