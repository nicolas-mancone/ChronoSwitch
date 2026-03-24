// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GameplayActors/ProximityDoor.h"

#include "Characters/ChronoSwitchCharacter.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Game/ChronoSwitchGameState.h"

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
	DoorPivotScene->SetupAttachment(SceneRoot);
	DoorMesh1->SetupAttachment(DoorPivotScene);
	DoorMesh2->SetupAttachment(DoorPivotScene);
	BoxColliderOpen->SetupAttachment(SceneRoot);
	BoxColliderClose1->SetupAttachment(SceneRoot);
	BoxColliderOpen->SetCollisionObjectType(ECC_WorldDynamic);
	BoxColliderOpen->SetCollisionResponseToAllChannels(ECR_Overlap);
	BoxColliderClose1->SetCollisionObjectType(ECC_WorldDynamic);
	BoxColliderClose1->SetCollisionResponseToAllChannels(ECR_Overlap);
	
	bReplicates = true;
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
	if (bUseGlobalSharedCounter)
	{
		if (AChronoSwitchGameState* GS = GetWorld()->GetGameState<AChronoSwitchGameState>())
		{
			GS->SharedPlayersAtDoor++;
			if (GS->SharedPlayersAtDoor >= RequiredPlayers)
			{
				GS->SharedPlayersAtDoor = RequiredPlayers;
				OpenDoor();
			}
		}
	}
	else
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
	if (!Cast<AChronoSwitchCharacter>(OtherActor))
		return;
	if (bUseGlobalSharedCounter)
	{
		if (AChronoSwitchGameState* GS = GetWorld()->GetGameState<AChronoSwitchGameState>())
		{
			GS->SharedPlayersAtDoor--;
		}
	}
	else
	{
		InPlayerCount--;
	}
}

void AProximityDoor::OnCloseBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Cast<AChronoSwitchCharacter>(OtherActor))
		return;
	if (BoxColliderOpen->IsOverlappingActor(OtherActor))
		return;
	if (bUseGlobalSharedCounter)
	{
		if (AChronoSwitchGameState* GS = GetWorld()->GetGameState<AChronoSwitchGameState>())
		{
			GS->SharedPlayersAtDoorExit--;
		}
	}
	else
	{
		OutPlayerCount--;
	}
}

void AProximityDoor::OnCloseEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Cast<AChronoSwitchCharacter>(OtherActor))
		return;
	if (BoxColliderOpen->IsOverlappingActor(OtherActor))
		return;
	if (bUseGlobalSharedCounter)
	{
		if (AChronoSwitchGameState* GS = GetWorld()->GetGameState<AChronoSwitchGameState>())
		{
			GS->SharedPlayersAtDoorExit++;
			if (GS->SharedPlayersAtDoorExit >= RequiredPlayersOnExit)
			{
				GS->SharedPlayersAtDoorExit = RequiredPlayersOnExit;
				CloseDoor();
			}
		}
	}
	else
	{
		OutPlayerCount++;
		if (OutPlayerCount >= RequiredPlayersOnExit)
		{
			OutPlayerCount = RequiredPlayersOnExit;
			CloseDoor();
		}
	}
}

void AProximityDoor::OnRep_InPlayerCount()
{
	if (InPlayerCount >= RequiredPlayers)
		OpenDoor();
}

void AProximityDoor::OnRep_OutPlayerCount()
{
	if (OutPlayerCount >= RequiredPlayersOnExit)
		CloseDoor();
}

void AProximityDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Registering variables for replication
	DOREPLIFETIME(AProximityDoor, OutPlayerCount);
	DOREPLIFETIME(AProximityDoor, InPlayerCount);
}
