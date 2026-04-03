// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GameplayActors/PowerCell.h"


// Sets default values
APowerCell::APowerCell()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APowerCell::BeginPlay()
{
	Super::BeginPlay();
	
}

void APowerCell::Interact_Implementation(ACharacter* Interactor)
{
	Super::Interact_Implementation(Interactor);
}

FText APowerCell::GetInteractPrompt_Implementation()
{
	return FText::FromString("Grab Power Cell");
}

// Called every frame
void APowerCell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

