// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerVisorWidget.h"

#include "Animation/AnimNode_TransitionPoseEvaluator.h"

void UPlayerVisorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	UpdateImages();
}

void UPlayerVisorWidget::TogglePauseMenu(bool bIsPaused)
{
	if (bIsPaused)
	{
		PauseMenuSwitcher->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		PauseMenuSwitcher->SetVisibility(ESlateVisibility::Hidden);
		PauseMenuSwitcher->SetActiveWidgetIndex(0);
	}
}

void UPlayerVisorWidget::SetPromptText(const FText& Text)
{
	if (PromptTextBlock)
	{
		PromptTextBlock->SetText(Text);
	}
}

void UPlayerVisorWidget::SetPromptVisibility(ESlateVisibility NewVisibility)
{
	if (PromptTextBlock)
	{
		PromptTextBlock->SetVisibility(NewVisibility);
	}
}

void UPlayerVisorWidget::UpdateMode(const EWidgetSwitchMode Mode)
{
	State.Mode = Mode;
	UpdateImages();
}

void UPlayerVisorWidget::UpdateCanSwitch(bool bCanSwitch)
{
	State.bCanSwitch = bCanSwitch;
	UpdateImages();
}

void UPlayerVisorWidget::UpdatePlayerTimeline(const uint8 PlayerTimeline)
{
	State.PlayerTimeline = PlayerTimeline;
	UpdateImages();
}

void UPlayerVisorWidget::UpdateOtherPlayerTimeline(const uint8 OtherPlayerTimeline)
{
	State.OtherPlayerTimeline = OtherPlayerTimeline;
	UpdateImages();
}

void UPlayerVisorWidget::UpdateImages()
{
	EWidgetSwitchMode Mode = State.bCanSwitch ? State.Mode : EWidgetSwitchMode::Disabled;
	if (SwitchModeImage && SwitchModeMap.Contains(Mode))
	{
		const FWidgetImageVariants& Images = SwitchModeMap[Mode];
		
		uint8 ModeTimeline;
		if (State.Mode == EWidgetSwitchMode::Friend)
			ModeTimeline = State.OtherPlayerTimeline;
		else
			ModeTimeline = State.PlayerTimeline;
		
		if (UTexture2D* Selected = ModeTimeline ? Images.FutureImage : Images.PastImage)
		{
			SwitchModeImage->SetBrushFromTexture(Selected);
		}
	}
	
	if (FriendModeImage)
	{
		if (UTexture2D* Selected = State.OtherPlayerTimeline ? FriendImages.FutureImage : FriendImages.PastImage)
		{
			FriendModeImage->SetBrushFromTexture(Selected);
		}
	}
}
