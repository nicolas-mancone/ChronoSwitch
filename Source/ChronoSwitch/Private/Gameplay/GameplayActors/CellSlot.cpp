// Fill out your copyright notice in the Description page of Project Settings.


#include "CellSlot.h"

#include "Components/BoxComponent.h"


// Sets default values
ACellSlot::ACellSlot()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	BoxCollider = CreateDefaultSubobject<UBoxComponent>("BoxCollider");
	BoxCollider->SetupAttachment(RootComponent);
	// Consider setting ignore to timeline0 collision response
}

// Called when the game starts or when spawned
void ACellSlot::BeginPlay()
{
	Super::BeginPlay();
	
	if (BoxCollider)
		BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ACellSlot::OnBeginOverlap);
}

void ACellSlot::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
		return;
	AFuturePhysicsTimelineActor* PhysicsActor = Cast<AFuturePhysicsTimelineActor>(OtherActor);
	if (!PhysicsActor)
		return;
	PhysicsActor->DetachFromCharacter();
	PhysicsActor->SetActorEnableCollision(false);
	PhysicsActor->DisableComponentsSimulatePhysics();
	MoveActorInPlace(PhysicsActor);
}

// Called every frame
void ACellSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}