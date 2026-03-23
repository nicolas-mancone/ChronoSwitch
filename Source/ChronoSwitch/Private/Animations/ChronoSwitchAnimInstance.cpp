#include "Animations/ChronoSwitchAnimInstance.h"
#include "Characters/ChronoSwitchCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UChronoSwitchAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ChronoCharacter = Cast<AChronoSwitchCharacter>(TryGetPawnOwner());
	if (ChronoCharacter)
	{
		MovementComponent = ChronoCharacter->GetCharacterMovement();
	}
}

void UChronoSwitchAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Executed on the Game Thread.
	// Only copy necessary data from game objects here. Defer math to the worker thread.
	if (ChronoCharacter && MovementComponent)
	{
		Velocity = ChronoCharacter->GetVelocity();
		AimRotation = ChronoCharacter->GetBaseAimRotation();
		
		LastActorRotation = ActorRotation;
		ActorRotation = ChronoCharacter->GetActorRotation();

		Acceleration = MovementComponent->GetCurrentAcceleration();
		bIsFalling = MovementComponent->IsFalling();
	}
}

void UChronoSwitchAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	// Executed on a Worker Thread.
	// Strictly for math operations. Direct access to game objects is unsafe here.
	
	GroundSpeed = Velocity.Size2D();
	bHasMovementInput = Acceleration.SizeSquared2D() > 0.0f;
	bShouldMove = bHasMovementInput && GroundSpeed > 3.0f; 

	if (bShouldMove)
	{
		MovementDirection = UKismetMathLibrary::NormalizedDeltaRotator(Velocity.ToOrientationRotator(), ActorRotation).Yaw;
	}
	
	VelocityZ = Velocity.Z;

	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, ActorRotation);
	AimPitch = DeltaRot.Pitch;
	AimYaw = DeltaRot.Yaw;

	FRotator YawDeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(ActorRotation, LastActorRotation);
	YawDelta = YawDeltaRot.Yaw / DeltaSeconds; 
}