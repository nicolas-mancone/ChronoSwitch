// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "Net/UnrealNetwork.h"
#include "DoorLever.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class ASlidingDoor;

UCLASS()
class CHRONOSWITCH_API ADoorLever : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADoorLever();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<ASlidingDoor> Door;
	
	virtual void Interact_Implementation(ACharacter* Interactor) override;
	virtual FText GetInteractPrompt_Implementation() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(ReplicatedUsing=OnRep_bIsPulled)
	bool bIsPulled = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BaseMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USceneComponent* LeverPivot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* LeverMesh;
	
	UFUNCTION(BlueprintImplementableEvent)
	void PullLeverUp();
	UFUNCTION(BlueprintImplementableEvent)
	void PullLeverDown();
	
	UFUNCTION()
	void OnRep_bIsPulled();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
