// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

#include "ProximityDoor.generated.h"

UCLASS()
class CHRONOSWITCH_API AProximityDoor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProximityDoor();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly)
	float OpeningOffset = 100;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
/*
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DoorPivotScene;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxColliderOpen;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxColliderClose;
*/	
	//UPROPERTY(ReplicatedUsing=OnRep_OutPlayerCount)
	uint8 OutPlayerCount = 0;
	//UPROPERTY(ReplicatedUsing=OnRep_InPlayerCount)
	uint8 InPlayerCount = 0;
	//UPROPERTY(Replicated)
	bool bIsDoorLocked = false;
	
	//UFUNCTION()
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
