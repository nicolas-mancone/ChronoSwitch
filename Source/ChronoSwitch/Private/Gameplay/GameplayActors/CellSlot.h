// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/TimelineActors/FuturePhysicsTimelineActor.h"
#include "Gameplay/TimelineActors/TimelineBaseActor.h"
#include "CellSlot.generated.h"

class UBoxComponent;

UCLASS()
class CHRONOSWITCH_API ACellSlot : public ATimelineBaseActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACellSlot();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	float Speed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxCollider;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Components")
	void MoveActorInPlace(AFuturePhysicsTimelineActor* MovingActor);
	
private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
