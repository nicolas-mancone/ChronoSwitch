// Fill out your copyright notice in the Description page of Project Settings.


#include  "Gameplay/GameplayActors/CellSlot.h"

#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/Actionable.h"


// Sets default values
ACellSlot::ACellSlot()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);
	
	BoxCollider = CreateDefaultSubobject<UBoxComponent>("BoxCollider");
	BoxCollider->SetupAttachment(SceneRoot);
	
	SetReplicates(true);
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
	if (bUsed)
		return;
	if (!HasAuthority())
		return;
	PhysicsActor = Cast<AFuturePhysicsTimelineActor>(OtherActor);
	OnRep_PhysicsActor();
}

void ACellSlot::OnRep_PhysicsActor()
{
	HandlePhysicsActorChange();
}

void ACellSlot::HandlePhysicsActorChange()
{
	if (PhysicsActor)
	{
		PhysicsActor->DetachFromCharacter();
		PhysicsActor->SetActorEnableCollision(false);
		PhysicsActor->DisableComponentsSimulatePhysics();
		bUsed = true;
		MoveActorInPlace(PhysicsActor);
	}
}

void ACellSlot::ActivateObject()
{
	if (!HasAuthority())
		return;
	if (ActionableActor->GetClass()->ImplementsInterface(UActionable::StaticClass()))
	{
		IActionable::Execute_Activate(ActionableActor, PhysicsActor);
	}
}

void ACellSlot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACellSlot, PhysicsActor);
	DOREPLIFETIME(ACellSlot, bUsed);
}

// Called every frame
void ACellSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}