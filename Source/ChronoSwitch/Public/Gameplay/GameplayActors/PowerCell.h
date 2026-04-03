// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Gameplay/TimelineActors/FuturePhysicsTimelineActor.h"
#include "PowerCell.generated.h"

UCLASS()
class CHRONOSWITCH_API APowerCell : public AFuturePhysicsTimelineActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APowerCell();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void Interact_Implementation(ACharacter* Interactor) override;
	virtual FText GetInteractPrompt_Implementation() override;
};
