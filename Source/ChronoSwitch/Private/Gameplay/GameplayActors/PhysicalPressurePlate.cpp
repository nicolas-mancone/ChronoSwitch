// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GameplayActors/PhysicalPressurePlate.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Gameplay/TimelineActors/PhysicsTimelineActor.h"
#include "Interfaces/Actionable.h"


// Sets default values
APhysicalPressurePlate::APhysicalPressurePlate()
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
void APhysicalPressurePlate::BeginPlay()
{
	Super::BeginPlay();
	
	if (BoxCollider)
	{
		BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &APhysicalPressurePlate::OnBeginOverlap);
		BoxCollider->OnComponentEndOverlap.AddDynamic(this, &APhysicalPressurePlate::OnEndOverlap);
	}
}

void APhysicalPressurePlate::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Cast<APhysicsTimelineActor>(OtherActor))
		return;
	Count++;
	if (Count == 1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Begin Collision"));
		OnPressurePlatePressed();
		ActivateObject();
	}
}

void APhysicalPressurePlate::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Cast<APhysicsTimelineActor>(OtherActor))
		return;
	Count--;
	if (Count == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("End Collision"));
		OnPressurePlateReleased();
		ActivateObject();
	}
}

void APhysicalPressurePlate::ActivateObject()
{
	if (!HasAuthority())
		return;
	for (AActor* Actor : ActionableActors)
	{
		if (Actor && Actor->GetClass()->ImplementsInterface(UActionable::StaticClass()))
		{
			IActionable::Execute_Activate(Actor, this);
		}
	}
}

// Called every frame
void APhysicalPressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

