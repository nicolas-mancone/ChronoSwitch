// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GameplayActors/ActionableDoor.h"

#include "Components/StaticMeshComponent.h"
#include "Gameplay/ActorComponents/DoorComponent.h"

// Sets default values
AActionableDoor::AActionableDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>("Door");
	DoorFrameMesh = CreateDefaultSubobject<UStaticMeshComponent>("DoorFrame");
	DoorComponent = CreateDefaultSubobject<UDoorComponent>("DoorComponent");
	
	DoorMesh->SetupAttachment(SceneRoot);
	DoorFrameMesh->SetupAttachment(SceneRoot);
	
	bReplicates = true;
}

// Called when the game starts or when spawned
void AActionableDoor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AActionableDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AActionableDoor::Activate_Implementation(AActor* ActorParam)
{
	if (!HasAuthority())
		return;
	if (!bIsOpen)
	{
		bIsOpen = true;
		OpenDoor();
	}
	else
	{
		bIsOpen = false;
		CloseDoor();
	}
}

void AActionableDoor::OnRep_bIsOpen()
{
	if (bIsOpen)
	{
		OpenDoor();
	}
	else
	{
		CloseDoor();
	}
}

void AActionableDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AActionableDoor, bIsOpen);
}