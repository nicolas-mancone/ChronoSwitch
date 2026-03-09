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
	
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	BoxCollider = CreateDefaultSubobject<UBoxComponent>("BoxCollider");
	StaticMesh->SetupAttachment(SceneRoot);
	BoxCollider->SetupAttachment(SceneRoot);
	BoxCollider->SetCollisionObjectType(ECC_WorldDynamic);
	BoxCollider->SetCollisionResponseToAllChannels(ECR_Overlap);
}

// Called when the game starts or when spawned
void APressurePlate::BeginPlay()
{
	Super::BeginPlay();
	
	if (BoxCollider)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Collision Setup"));
		BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &APressurePlate::OnBeginOverlap);
		BoxCollider->OnComponentEndOverlap.AddDynamic(this, &APressurePlate::OnEndOverlap);
	}
}

void APressurePlate::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayersOnPlate++;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Begin Collision"));
	if (PlayersOnPlate == 1)
	{
		if (Cast<AChronoSwitchCharacter>(OtherActor))
		{
			if (AChronoSwitchGameState* GS = GetWorld()->GetGameState<AChronoSwitchGameState>())
			{
				GS->SetGlobalTimeline(TimelineToSet);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Switched Timeline"));
			}
		}
	}
}

void APressurePlate::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PlayersOnPlate--;
	if (PlayersOnPlate == 0)
	{
		if (Cast<AChronoSwitchCharacter>(OtherActor))
		{
			if (AChronoSwitchGameState* GS = GetWorld()->GetGameState<AChronoSwitchGameState>())
			{
				GS->SetGlobalTimeline(TimelineToSet == 0 ? 1 : 0 );
			}
		}
	}
}

// Called every frame
void APressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}