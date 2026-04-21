// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/TimelineActors/ChronalAnchorGrid.h"
#include "Characters/ChronoSwitchCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Game/ChronoSwitchPlayerState.h"


// Sets default values
AChronalAnchorGrid::AChronalAnchorGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	BarrierMesh = CreateDefaultSubobject<UStaticMeshComponent>("BarrierMesh");
	GridBorder1 = CreateDefaultSubobject<UStaticMeshComponent>("GridBorder1");
	GridBorder2 = CreateDefaultSubobject<UStaticMeshComponent>("GridBorder2");
	EntranceDirectionArrow = CreateDefaultSubobject<UArrowComponent>("EntranceDirectionArrow");
	GridBorder1->SetupAttachment(SceneRoot);
	GridBorder2->SetupAttachment(SceneRoot);
	BarrierMesh->SetupAttachment(SceneRoot);
	EntranceDirectionArrow->SetupAttachment(SceneRoot);
	
	BarrierMesh->SetGenerateOverlapEvents(true);
	BarrierMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	BarrierMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
}

// Called when the game starts or when spawned
void AChronalAnchorGrid::BeginPlay()
{
	Super::BeginPlay();
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Forward Vector:\nx: %.2f, y: %.2f, z: %.2f"), GetActorForwardVector().X, GetActorForwardVector().Y, GetActorForwardVector().Z));
	
	if (BarrierMesh)
	{
		BarrierMesh->OnComponentBeginOverlap.AddDynamic(this, &AChronalAnchorGrid::OnBeginOverlap);
		BarrierMesh->OnComponentEndOverlap.AddDynamic(this, &AChronalAnchorGrid::OnEndOverlap);
	}
}

void AChronalAnchorGrid::OnBeginOverlap(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (!HasAuthority()) return;

	AChronoSwitchCharacter* Player = Cast<AChronoSwitchCharacter>(Other);
	if (!Player) return;
	
	if (OtherComp != Player->GetCapsuleComponent()) return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("OnBeginOverlap"));

	FVector EntryNormal = GetInteractionNormal(Player);
	StoredEntryNormals.Add(Player, EntryNormal);
}

void AChronalAnchorGrid::OnEndOverlap(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex)
{
	if (!HasAuthority()) return;

	AChronoSwitchCharacter* Player = Cast<AChronoSwitchCharacter>(Other);
	if (!Player) return;
	
	if (OtherComp != Player->GetCapsuleComponent()) return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("OnEndOverlap"));

	// The map returns a pointer to the value
	FVector* EntryNormalPtr = StoredEntryNormals.Find(Player);
	if (!EntryNormalPtr)
		return;

	FVector EntryNormal = *EntryNormalPtr;
	FVector ExitNormal = GetInteractionNormal(Player);
	
	// Check if we crossed THROUGH the mesh
	// Dot product of opposite vectors is -1. We check if it's negative enough.
	if (FVector::DotProduct(EntryNormal, ExitNormal) < -0.1f)
	{
		// Determine direction relative to the Arrow
		float DirectionDot = FVector::DotProduct(EntryNormal, EntranceDirectionArrow->GetForwardVector());
		
		// Player is entering the C.A.G Zone (Moving in direction of Arrow)
		if (DirectionDot < 0.0f)
		{
			if (AChronoSwitchPlayerState* PS = Cast<AChronoSwitchPlayerState>(Player->GetPlayerState()))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Entered"));
				if (EnteringCrossingSettings.TargetForcedTimeline != EForcedTimeline::None)
				{
					PS->RequestTimelineChange(static_cast<uint8>(EnteringCrossingSettings.TargetForcedTimeline), true);
				}
				if (EnteringCrossingSettings.VisorMode != ECrossingEffectMode::None)
				{
					PS->RequestVisorStateChange(static_cast<bool>(EnteringCrossingSettings.VisorMode));
				}
				if (EnteringCrossingSettings.SwitchMode != ECrossingEffectMode::None)
				{
					PS->RequestCanSwitchTimelineChange(static_cast<bool>(EnteringCrossingSettings.SwitchMode));
				}
				
				ManageSoundOnCrossing();
			}
		}
		// Player is exiting the C.A.G Zone (Moving against direction of Arrow)
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Exited"));
			if (AChronoSwitchPlayerState* PS = Cast<AChronoSwitchPlayerState>(Player->GetPlayerState()))
			{
				if (ExitingCrossingSettings.TargetForcedTimeline != EForcedTimeline::None)
				{
					PS->RequestTimelineChange(static_cast<uint8>(ExitingCrossingSettings.TargetForcedTimeline), true);
				}
				if (ExitingCrossingSettings.VisorMode != ECrossingEffectMode::None)
				{
					PS->RequestVisorStateChange(static_cast<bool>(ExitingCrossingSettings.VisorMode));
				}
				if (ExitingCrossingSettings.SwitchMode != ECrossingEffectMode::None)
				{
					PS->RequestCanSwitchTimelineChange(static_cast<bool>(ExitingCrossingSettings.SwitchMode));
				}
				
				ManageSoundOnCrossing();
			}
		}
	}

	StoredEntryNormals.Remove(Player);
}

FVector AChronalAnchorGrid::GetInteractionNormal(const AActor* Actor) const
{
	if (!BarrierMesh || !Actor) return FVector::ZeroVector;

	FVector ClosestPoint;
	// Finds the point on the mesh surface closest to the actor
	BarrierMesh->GetClosestPointOnCollision(Actor->GetActorLocation(), ClosestPoint);
	
	return (Actor->GetActorLocation() - ClosestPoint).GetSafeNormal();
}

// Called every frame
void AChronalAnchorGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
