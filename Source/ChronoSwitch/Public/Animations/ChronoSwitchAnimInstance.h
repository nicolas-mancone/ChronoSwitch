#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ChronoSwitchAnimInstance.generated.h"

class AChronoSwitchCharacter;
class UCharacterMovementComponent;

UCLASS()
class CHRONOSWITCH_API UChronoSwitchAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "References")
	AChronoSwitchCharacter* ChronoCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "References")
	UCharacterMovementComponent* MovementComponent;

	FVector Velocity;
	FRotator ActorRotation;
	FRotator AimRotation;
	FRotator LastActorRotation;
	FVector Acceleration;

	
	/** Horizontal speed used for Idle/Walk/Run evaluation. */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	float GroundSpeed;

	/** Movement angle relative to the actor's forward vector [-180, 180]. */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	float MovementDirection;

	/** Evaluates if the character is actively trying to move (avoids sliding animations on deceleration). */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	bool bShouldMove;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	bool bHasMovementInput;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	float VelocityZ;

	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Aiming")
	float AimPitch;

	UPROPERTY(BlueprintReadOnly, Category = "Aiming")
	float AimYaw;

	/** Rate of change in the actor's yaw rotation. Used for turn-in-place logic. */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	float YawDelta;
};