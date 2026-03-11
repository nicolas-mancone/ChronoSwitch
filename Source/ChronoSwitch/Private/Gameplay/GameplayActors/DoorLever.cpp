// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GameplayActors/DoorLever.h"
#include "Gameplay/GameplayActors/SlidingDoor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Character.h"


// Sets default values
ADoorLever::ADoorLever()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	BaseMesh->SetupAttachment(SceneRoot);
	LeverPivot = CreateDefaultSubobject<USceneComponent>("StaticMesh");
	LeverPivot->SetupAttachment(SceneRoot);
	LeverMesh = CreateDefaultSubobject<UStaticMeshComponent>("LeverMesh");
	LeverMesh->SetupAttachment(LeverPivot);
	
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ADoorLever::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADoorLever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADoorLever::Interact_Implementation(ACharacter* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Interaction"));
	if (!HasAuthority())
		return;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("Authority Interaction"));
	if (!bIsPulled)
	{
		bIsPulled = true;
		Door->OpenDoor();
		PullLeverDown();
	}
	else
	{
		bIsPulled = false;
		Door->CloseDoor();
		PullLeverUp();
	}
}

FText ADoorLever::GetInteractPrompt_Implementation()
{
	return FText::FromString("Toggle Lever"); 
}

void ADoorLever::OnRep_bIsPulled()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("OnRep_bIsPulled"));
	if (bIsPulled)
	{
		Door->OpenDoor();
		PullLeverDown();
	}
	else
	{
		Door->CloseDoor();
		PullLeverUp();
	}
}


void ADoorLever::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ADoorLever, bIsPulled);
}


