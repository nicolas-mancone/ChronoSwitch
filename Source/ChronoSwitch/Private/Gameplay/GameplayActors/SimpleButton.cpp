// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GameplayActors/SimpleButton.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Interfaces/Actionable.h"

// Sets default values
ASimpleButton::ASimpleButton()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	BaseMesh->SetupAttachment(SceneRoot);
	
	bReplicates = true;
}

// Called when the game starts or when spawned
void ASimpleButton::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASimpleButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASimpleButton::Interact_Implementation(ACharacter* Interactor)
{
	if (!HasAuthority())
		return;
	
	if (ActionableActor->GetClass()->ImplementsInterface(UActionable::StaticClass()))
	{
		IActionable::Execute_Activate(ActionableActor, this);
		// PressButton() is not called on client right now.
		PressButton();
	}
}

FText ASimpleButton::GetInteractPrompt_Implementation()
{
	return FText::FromString("Press F to activate button"); 
}

