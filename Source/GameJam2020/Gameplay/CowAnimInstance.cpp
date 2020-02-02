// Fill out your copyright notice in the Description page of Project Settings.

#include "CowAnimInstance.h"

#include "Gameplay/Cow.h"
#include "GameFramework/PawnMovementComponent.h"


void UCowAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	Cow = Cast<ACow>(GetOwningActor());
	MovementComponent = Cow->GetMovementComponent();
	RunThreshold = 400.0f;
}
void UCowAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Update speed value
	if (MovementComponent)
	{
		Speed = MovementComponent->Velocity.Size();
	}

	// Set bools based on speed
	if (Speed <= 0.0f)
	{
		IsIdle = true;
		IsRunning = false;
		IsWalking = false;
	}
	else if (Speed >= 0.0f && Speed < RunThreshold)
	{
		IsWalking = true;
		IsRunning = false;
		IsIdle = false;
	}
	else
	{
		IsRunning = true;
		IsWalking = false;
		IsIdle = false;
	}
}