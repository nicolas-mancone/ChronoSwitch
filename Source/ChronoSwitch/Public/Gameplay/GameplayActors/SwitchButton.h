// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/TimelineActors/TimelineBaseActor.h"
#include "Net/UnrealNetwork.h"
#include "SwitchButton.generated.h"

UCLASS()
class CHRONOSWITCH_API ASwitchButton : public ATimelineBaseActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASwitchButton();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void Interact_Implementation(ACharacter* Interactor) override;
	virtual FText GetInteractPrompt_Implementation() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnButtonPressed();
	
	UPROPERTY(EditDefaultsOnly, Replicated, Category="SwitchButton")
	uint8 CurrentButtonTimeline = 0;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
