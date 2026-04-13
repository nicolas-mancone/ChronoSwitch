// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GameplayActors/CellPipeEnd.h"


// Sets default values
ACellPipeEnd::ACellPipeEnd()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ACellPipeEnd::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACellPipeEnd::Activate_Implementation(AActor* ActorParam)
{
	PhysicsActor = Cast<AFuturePhysicsTimelineActor>(ActorParam);
	OnRep_PhysicsActor();
}

void ACellPipeEnd::OnRep_PhysicsActor()
{
	if (PhysicsActor)
	{
		OnActivation(PhysicsActor);
	}
}

void ACellPipeEnd::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACellPipeEnd, PhysicsActor);
}

// Called every frame
void ACellPipeEnd::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

