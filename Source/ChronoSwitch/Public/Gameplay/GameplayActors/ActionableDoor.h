// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Actionable.h"
#include "Net/UnrealNetwork.h"
#include "ActionableDoor.generated.h"

class UStaticMeshComponent;
class UDoorComponent;

UCLASS()
class CHRONOSWITCH_API AActionableDoor : public AActor, public IActionable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AActionableDoor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void Activate_Implementation(AActor* ActorParam) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OpenDoor();
	UFUNCTION(BlueprintImplementableEvent)
	void CloseDoor();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(ReplicatedUsing=OnRep_bIsOpen)
	bool bIsOpen = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* DoorMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* DoorFrameMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UDoorComponent* DoorComponent;
	
	UFUNCTION()
	void OnRep_bIsOpen();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
