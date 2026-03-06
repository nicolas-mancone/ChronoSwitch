// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GameplayActors/ProximityDoor.h"

#include "Characters/ChronoSwitchCharacter.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AProximityDoor::AProximityDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	DoorPivotScene = CreateDefaultSubobject<USceneComponent>("DoorPivotScene");
	DoorMesh1 = CreateDefaultSubobject<UStaticMeshComponent>("DoorMesh1");
	DoorMesh2 = CreateDefaultSubobject<UStaticMeshComponent>("DoorMesh2");
	BoxColliderOpen = CreateDefaultSubobject<UBoxComponent>("BoxColliderOpen");
	BoxColliderClose1 = CreateDefaultSubobject<UBoxComponent>("BoxColliderClose1");
	//BoxColliderClose2 = CreateDefaultSubobject<UBoxComponent>("BoxColliderClose2");
	//BoxColliderClose3 = CreateDefaultSubobject<UBoxComponent>("BoxColliderClose3");
	DoorPivotScene->SetupAttachment(SceneRoot);
	DoorMesh1->SetupAttachment(DoorPivotScene);
	DoorMesh2->SetupAttachment(DoorPivotScene);
	BoxColliderOpen->SetupAttachment(SceneRoot);
	BoxColliderClose1->SetupAttachment(SceneRoot);
	BoxColliderOpen->SetCollisionObjectType(ECC_WorldDynamic);
	BoxColliderOpen->SetCollisionResponseToAllChannels(ECR_Overlap);
	BoxColliderClose1->SetCollisionObjectType(ECC_WorldDynamic);
	BoxColliderClose1->SetCollisionResponseToAllChannels(ECR_Overlap);
	//BoxColliderClose2->SetupAttachment(SceneRoot);
	//BoxColliderClose3->SetupAttachment(SceneRoot);
	
	SetReplicates(true);
}

// Called when the game starts or when spawned
void AProximityDoor::BeginPlay()
{
	Super::BeginPlay();
	
	if (BoxColliderOpen && BoxColliderClose1)
	{
		BoxColliderOpen->OnComponentBeginOverlap.AddDynamic(this, &AProximityDoor::OnOpenBeginOverlap);
		BoxColliderOpen->OnComponentEndOverlap.AddDynamic(this, &AProximityDoor::OnOpenEndOverlap);
		
		BoxColliderClose1->OnComponentBeginOverlap.AddDynamic(this, &AProximityDoor::OnCloseBeginOverlap);
		BoxColliderClose1->OnComponentEndOverlap.AddDynamic(this, &AProximityDoor::OnCloseEndOverlap);
		//BoxColliderClose2->OnComponentBeginOverlap.AddDynamic(this, &AProximityDoor::OnCloseBeginOverlap);
		//BoxColliderClose2->OnComponentEndOverlap.AddDynamic(this, &AProximityDoor::OnCloseEndOverlap);
		//BoxColliderClose3->OnComponentBeginOverlap.AddDynamic(this, &AProximityDoor::OnCloseBeginOverlap);
		//BoxColliderClose3->OnComponentEndOverlap.AddDynamic(this, &AProximityDoor::OnCloseEndOverlap);
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
	if (Cast<AChronoSwitchCharacter>(OtherActor))
	{
		InPlayerCount++;
		if (InPlayerCount >= RequiredPlayers)
		{
			InPlayerCount = RequiredPlayers;
			OpenDoor();
		}
	}
}

void AProximityDoor::OnOpenEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AChronoSwitchCharacter>(OtherActor))
	{
		InPlayerCount--;
	}
}

void AProximityDoor::OnCloseBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AChronoSwitchCharacter>(OtherActor))
	{
		OutPlayerCount++;
		if (OutPlayerCount >= RequiredPlayers)
		{
			OutPlayerCount = RequiredPlayers;
			CloseDoor();
		}
	}
}

void AProximityDoor::OnCloseEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AChronoSwitchCharacter>(OtherActor))
	{
		if (BoxColliderOpen->IsOverlappingActor(OtherActor))
			OutPlayerCount--;
	}
}

void AProximityDoor::OnRep_InPlayerCount()
{
	if (InPlayerCount >= RequiredPlayers)
		OpenDoor();
}

void AProximityDoor::OnRep_OutPlayerCount()
{
	if (OutPlayerCount >= RequiredPlayers)
		CloseDoor();
}

void AProximityDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Registering variables for replication
	DOREPLIFETIME(AProximityDoor, OutPlayerCount);
	DOREPLIFETIME(AProximityDoor, InPlayerCount);
}
