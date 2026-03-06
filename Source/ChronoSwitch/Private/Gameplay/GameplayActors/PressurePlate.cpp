// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GameplayActors/PressurePlate.h"

#include "Characters/ChronoSwitchCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Game/ChronoSwitchGameState.h"
#include "Game/ChronoSwitchPlayerState.h"

// Sets default values
APressurePlate::APressurePlate()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	BoxCollider = CreateDefaultSubobject<UBoxComponent>("BoxCollider");
	StaticMesh->SetupAttachment(GetRootComponent());
	BoxCollider->SetupAttachment(GetRootComponent());
	BoxCollider->SetCollisionObjectType(ECC_WorldDynamic);
	BoxCollider->SetCollisionResponseToAllChannels(ECR_Overlap);
}

// Called when the game starts or when spawned
void APressurePlate::BeginPlay()
{
	Super::BeginPlay();
	
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &APressurePlate::OnBeginOverlap);
	BoxCollider->OnComponentEndOverlap.AddDynamic(this, &APressurePlate::OnEndOverlap);
}

void APressurePlate::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AChronoSwitchCharacter>(OtherActor))
	{
		if (AChronoSwitchGameState* GS = GetWorld()->GetGameState<AChronoSwitchGameState>())
		{
			GS->SetGlobalTimeline(TimelineToSet);
		}
	}
}

void APressurePlate::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AChronoSwitchCharacter>(OtherActor))
	{
		if (AChronoSwitchGameState* GS = GetWorld()->GetGameState<AChronoSwitchGameState>())
		{
			GS->SetGlobalTimeline(TimelineToSet == 0 ? 1 : 0 );
		}
	}
}

// Called every frame
void APressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}