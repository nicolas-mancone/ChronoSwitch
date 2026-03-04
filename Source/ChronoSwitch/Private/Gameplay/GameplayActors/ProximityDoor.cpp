// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GameplayActors/ProximityDoor.h"




// Sets default values
AProximityDoor::AProximityDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AProximityDoor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProximityDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
/*
void AProximityDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Registering variables for replication
	DOREPLIFETIME(AProximityDoor, OutPlayerCount);
	DOREPLIFETIME(AProximityDoor, InPlayerCount);
	DOREPLIFETIME(AProximityDoor, bIsDoorLocked);
}
*/