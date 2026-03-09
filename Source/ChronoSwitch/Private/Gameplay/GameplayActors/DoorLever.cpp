// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GameplayActors/DoorLever.h"
#include "Gameplay/GameplayActors/SlidingDoor.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
ADoorLever::ADoorLever()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	LeverMesh = CreateDefaultSubobject<UStaticMeshComponent>("LeverMesh");
	LeverMesh->SetupAttachment(SceneRoot);
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	BaseMesh->SetupAttachment(SceneRoot);
}

// Called when the game starts or when spawned
void ADoorLever::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoorLever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADoorLever::Interact_Implementation(ACharacter* Interactor)
{
	if (!bIsPulled)
	{
		bIsPulled = true;
		Door->OpenDoor();
		PullLeverDown();
	}
	else
	{
		bIsPulled = false;
		Door->CloseDoor();
		PullLeverUp();
	}
}

FText ADoorLever::GetInteractPrompt_Implementation()
{
	return FText::FromString("Toggle Lever"); 
}

