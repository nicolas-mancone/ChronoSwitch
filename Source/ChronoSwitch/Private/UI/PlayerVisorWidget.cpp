// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerVisorWidget.h"

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
