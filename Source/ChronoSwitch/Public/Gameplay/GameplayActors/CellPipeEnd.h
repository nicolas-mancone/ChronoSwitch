// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/TimelineActors/FuturePhysicsTimelineActor.h"
#include "Interfaces/Actionable.h"
#include "Net/UnrealNetwork.h"
#include "CellPipeEnd.generated.h"

UCLASS()
class CHRONOSWITCH_API ACellPipeEnd : public AActor, public IActionable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACellPipeEnd();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(ReplicatedUsing=OnRep_PhysicsActor)
	AFuturePhysicsTimelineActor* PhysicsActor;
	
	virtual void Activate_Implementation(AActor* ActorParam) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnActivation(AFuturePhysicsTimelineActor* NewPhysicsActor);
	
	UFUNCTION()
	void OnRep_PhysicsActor();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
