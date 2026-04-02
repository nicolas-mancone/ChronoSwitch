// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlidingDoor.generated.h"

class UStaticMeshComponent;
class UDoorComponent;

UCLASS()
class CHRONOSWITCH_API ASlidingDoor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASlidingDoor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OpenDoor();
	UFUNCTION(BlueprintImplementableEvent)
	void CloseDoor();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* DoorMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* DoorFrameMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UDoorComponent* DoorComponent;
};
