// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GameplayActors/ProximityDoor.h"

#include "Characters/ChronoSwitchCharacter.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Gameplay/ActorComponents/DoorComponent.h"

// Sets default values
AProximityDoor::AProximityDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	DoorComponent = CreateDefaultSubobject<UDoorComponent>("DoorComponent");
	DoorPivotScene = CreateDefaultSubobject<USceneComponent>("DoorPivotScene");
	DoorMesh1 = CreateDefaultSubobject<UStaticMeshComponent>("DoorMesh1");
	DoorMesh2 = CreateDefaultSubobject<UStaticMeshComponent>("DoorMesh2");
	BoxColliderOpen = CreateDefaultSubobject<UBoxComponent>("BoxColliderOpen");
	BoxColliderClose = CreateDefaultSubobject<UBoxComponent>("BoxColliderClose");
	DoorPivotScene->SetupAttachment(SceneRoot);
	DoorMesh1->SetupAttachment(DoorPivotScene);
	DoorMesh2->SetupAttachment(DoorPivotScene);
	BoxColliderOpen->SetupAttachment(SceneRoot);
	BoxColliderClose->SetupAttachment(SceneRoot);
	BoxColliderOpen->SetCollisionObjectType(ECC_WorldDynamic);
	BoxColliderOpen->SetCollisionResponseToAllChannels(ECR_Overlap);
	BoxColliderClose->SetCollisionObjectType(ECC_WorldDynamic);
	BoxColliderClose->SetCollisionResponseToAllChannels(ECR_Overlap);
}

// Called when the game starts or when spawned
void AProximityDoor::BeginPlay()
{
	Super::BeginPlay();
	
	if (BoxColliderOpen && BoxColliderClose)
	{
		BoxColliderOpen->OnComponentBeginOverlap.AddDynamic(this, &AProximityDoor::OnOpenBeginOverlap);
		BoxColliderOpen->OnComponentEndOverlap.AddDynamic(this, &AProximityDoor::OnOpenEndOverlap);
		
		BoxColliderClose->OnComponentBeginOverlap.AddDynamic(this, &AProximityDoor::OnCloseBeginOverlap);
		BoxColliderClose->OnComponentEndOverlap.AddDynamic(this, &AProximityDoor::OnCloseEndOverlap);
	}
}

// Called every frame
void AProximityDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProximityDoor::OnOpenBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Cast<AChronoSwitchCharacter>(OtherActor))
		return;
	InPlayerCount++;
	if (InPlayerCount >= RequiredPlayers)
	{
		InPlayerCount = RequiredPlayers;
		OpenDoor();
	}
}

void AProximityDoor::OnOpenEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Cast<AChronoSwitchCharacter>(OtherActor))
		return;
	InPlayerCount--;
}

void AProximityDoor::OnCloseBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Cast<AChronoSwitchCharacter>(OtherActor))
		return;
	if (BoxColliderOpen->IsOverlappingActor(OtherActor))
		return;
	OutPlayerCount--;
}

void AProximityDoor::OnCloseEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Cast<AChronoSwitchCharacter>(OtherActor))
		return;
	if (BoxColliderOpen->IsOverlappingActor(OtherActor))
		return;
	OutPlayerCount++;
	if (OutPlayerCount >= RequiredPlayers)
	{
		OutPlayerCount = RequiredPlayers;
		CloseDoor();
	}
}