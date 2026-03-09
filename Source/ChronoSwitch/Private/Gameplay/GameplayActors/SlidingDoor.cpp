// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GameplayActors/SlidingDoor.h"

#include "Components/StaticMeshComponent.h"

// Sets default values
ASlidingDoor::ASlidingDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>("Door");
	DoorFrameMesh = CreateDefaultSubobject<UStaticMeshComponent>("DoorFrame");
	
	DoorMesh->SetupAttachment(SceneRoot);
	DoorFrameMesh->SetupAttachment(SceneRoot);
	
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASlidingDoor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASlidingDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}