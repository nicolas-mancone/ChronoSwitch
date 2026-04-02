// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GameplayActors/PairedProximityDoors.h"

#include "Characters/ChronoSwitchCharacter.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Gameplay/ActorComponents/DoorComponent.h"

// Sets default values
APairedProximityDoors::APairedProximityDoors()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	DoorComponent = CreateDefaultSubobject<UDoorComponent>("DoorComponent");
	DoorPivotSceneD1 = CreateDefaultSubobject<USceneComponent>("DoorPivotSceneD1");
	DoorPivotSceneD2 = CreateDefaultSubobject<USceneComponent>("DoorPivotSceneD2");
	
	//Door 1
	DoorMesh1D1 = CreateDefaultSubobject<UStaticMeshComponent>("DoorMesh1D1");
	DoorMesh2D1 = CreateDefaultSubobject<UStaticMeshComponent>("DoorMesh2D1");
	BoxColliderOpenD1 = CreateDefaultSubobject<UBoxComponent>("BoxColliderOpenD1");
	BoxColliderCloseD1 = CreateDefaultSubobject<UBoxComponent>("BoxColliderCloseD1");
	
	DoorPivotSceneD1->SetupAttachment(SceneRoot);
	DoorMesh1D1->SetupAttachment(DoorPivotSceneD1);
	DoorMesh2D1->SetupAttachment(DoorPivotSceneD1);
	BoxColliderOpenD1->SetupAttachment(DoorPivotSceneD1);
	BoxColliderCloseD1->SetupAttachment(DoorPivotSceneD1);
	
	BoxColliderOpenD1->SetCollisionObjectType(ECC_WorldDynamic);
	BoxColliderOpenD1->SetCollisionResponseToAllChannels(ECR_Overlap);
	BoxColliderCloseD1->SetCollisionObjectType(ECC_WorldDynamic);
	BoxColliderCloseD1->SetCollisionResponseToAllChannels(ECR_Overlap);
	
	//Door 2
	DoorMesh1D2 = CreateDefaultSubobject<UStaticMeshComponent>("DoorMesh1D2");
	DoorMesh2D2 = CreateDefaultSubobject<UStaticMeshComponent>("DoorMesh2D2");
	BoxColliderOpenD2 = CreateDefaultSubobject<UBoxComponent>("BoxColliderOpenD2");
	BoxColliderCloseD2 = CreateDefaultSubobject<UBoxComponent>("BoxColliderCloseD2");
	
	DoorPivotSceneD2->SetupAttachment(SceneRoot);
	DoorMesh1D2->SetupAttachment(DoorPivotSceneD2);
	DoorMesh2D2->SetupAttachment(DoorPivotSceneD2);
	BoxColliderOpenD2->SetupAttachment(DoorPivotSceneD2);
	BoxColliderCloseD2->SetupAttachment(DoorPivotSceneD2);
	
	BoxColliderOpenD2->SetCollisionObjectType(ECC_WorldDynamic);
	BoxColliderOpenD2->SetCollisionResponseToAllChannels(ECR_Overlap);
	BoxColliderCloseD2->SetCollisionObjectType(ECC_WorldDynamic);
	BoxColliderCloseD2->SetCollisionResponseToAllChannels(ECR_Overlap);
}

// Called when the game starts or when spawned
void APairedProximityDoors::BeginPlay()
{
	Super::BeginPlay();
	
	if (BoxColliderOpenD1 && BoxColliderCloseD1 && BoxColliderOpenD2 && BoxColliderCloseD2)
	{
		BoxColliderOpenD1->OnComponentBeginOverlap.AddDynamic(this, &APairedProximityDoors::OnOpenBeginOverlap);
		BoxColliderOpenD1->OnComponentEndOverlap.AddDynamic(this, &APairedProximityDoors::OnOpenEndOverlap);
		
		BoxColliderCloseD1->OnComponentBeginOverlap.AddDynamic(this, &APairedProximityDoors::OnCloseBeginOverlap);
		BoxColliderCloseD1->OnComponentEndOverlap.AddDynamic(this, &APairedProximityDoors::OnCloseEndOverlap);
		
		BoxColliderOpenD2->OnComponentBeginOverlap.AddDynamic(this, &APairedProximityDoors::OnOpenBeginOverlap);
		BoxColliderOpenD2->OnComponentEndOverlap.AddDynamic(this, &APairedProximityDoors::OnOpenEndOverlap);
		
		BoxColliderCloseD2->OnComponentBeginOverlap.AddDynamic(this, &APairedProximityDoors::OnCloseBeginOverlap);
		BoxColliderCloseD2->OnComponentEndOverlap.AddDynamic(this, &APairedProximityDoors::OnCloseEndOverlap);
	}
}

void APairedProximityDoors::OnOpenBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Cast<AChronoSwitchCharacter>(OtherActor))
		return;
	if (OverlappedComponent == BoxColliderOpenD1)
	{
		InPlayerCountD1++;
		if (InPlayerCountD1 > 0 && InPlayerCountD2 > 0)
		{
			OpenDoor();
		}
	}
	else if (OverlappedComponent == BoxColliderOpenD2)
	{
		InPlayerCountD2++;
		if (InPlayerCountD1 > 0 && InPlayerCountD2 > 0)
		{
			OpenDoor();
		}
	}
}

void APairedProximityDoors::OnOpenEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Cast<AChronoSwitchCharacter>(OtherActor))
		return;
	if (OverlappedComponent == BoxColliderOpenD1)
	{
		InPlayerCountD1--;
	}
	else if (OverlappedComponent == BoxColliderOpenD2)
	{
		InPlayerCountD2--;
	}
}

void APairedProximityDoors::OnCloseBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Cast<AChronoSwitchCharacter>(OtherActor))
		return;
	if (OverlappedComponent == BoxColliderCloseD1)
	{
		if (BoxColliderOpenD1->IsOverlappingActor(OtherActor))
			return;
		OutPlayerCountD1--;
	}
	else if (OverlappedComponent == BoxColliderCloseD2)
	{
		if (BoxColliderOpenD2->IsOverlappingActor(OtherActor))
			return;
		OutPlayerCountD2--;
	}
}

void APairedProximityDoors::OnCloseEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Cast<AChronoSwitchCharacter>(OtherActor))
		return;
	if (OverlappedComponent == BoxColliderCloseD1)
	{
		if (BoxColliderOpenD1->IsOverlappingActor(OtherActor))
			return;
		OutPlayerCountD1++;
		if (OutPlayerCountD1 > 0 && OutPlayerCountD2 > 0)
		{
			CloseDoor();
		}
	}
	else if (OverlappedComponent == BoxColliderCloseD2)
	{
		if (BoxColliderOpenD2->IsOverlappingActor(OtherActor))
			return;
		OutPlayerCountD2++;
		if (OutPlayerCountD1 > 0 && OutPlayerCountD2 > 0)
		{
			CloseDoor();
		}
	}
}

// Called every frame
void APairedProximityDoors::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}