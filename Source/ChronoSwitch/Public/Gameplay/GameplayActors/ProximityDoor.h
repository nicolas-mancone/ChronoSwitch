// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "ProximityDoor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;
class AChronoSwitchCharacter;

UCLASS()
class CHRONOSWITCH_API AProximityDoor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProximityDoor();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
	float OpeningOffset = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
	float OpeningSpeed = 1;
	UPROPERTY(EditAnywhere, Category="Door")
	uint8 RequiredPlayers = 2;
	UPROPERTY(EditAnywhere, Category="Door")
	uint8 RequiredPlayersOnExit = 2;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DoorPivotScene;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxColliderOpen;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxColliderClose1;

	UPROPERTY(ReplicatedUsing=OnRep_OutPlayerCount)
	uint8 OutPlayerCount = 0;
	UPROPERTY(ReplicatedUsing=OnRep_InPlayerCount)
	uint8 InPlayerCount = 0;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OpenDoor();
	UFUNCTION(BlueprintImplementableEvent)
	void CloseDoor();
	
	UFUNCTION()
	void OnOpenBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOpenEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnCloseBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCloseEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void OnRep_OutPlayerCount();
	UFUNCTION()
	void OnRep_InPlayerCount();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
