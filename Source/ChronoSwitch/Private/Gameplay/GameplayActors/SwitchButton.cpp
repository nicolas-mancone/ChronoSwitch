// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GameplayActors/SwitchButton.h"

#include "Game/ChronoSwitchGameState.h"


// Sets default values
ASwitchButton::ASwitchButton()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASwitchButton::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASwitchButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASwitchButton::Interact_Implementation(ACharacter* Interactor)
{
	OnButtonPressed();
	AChronoSwitchGameState* GS = GetWorld()->GetGameState<AChronoSwitchGameState>();
	CurrentButtonTimeline = CurrentButtonTimeline ? 0 : 1;
	GS->SetGlobalTimeline(CurrentButtonTimeline);
}

FText ASwitchButton::GetInteractPrompt_Implementation()
{
	return FText::FromString("Press F to press button");
}

void ASwitchButton::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASwitchButton, CurrentButtonTimeline);
}