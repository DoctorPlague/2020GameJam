// Fill out your copyright notice in the Description page of Project Settings.


#include "Cow.h"

#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ACow::ACow()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACow::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACow::SetMovementEnabled(bool _bEnabled)
{
	bCanMove = _bEnabled;
	EMovementMode NewMovement = EMovementMode::MOVE_None;
	if (_bEnabled)
		NewMovement = EMovementMode::MOVE_Walking;
	GetCharacterMovement()->SetMovementMode(NewMovement);
}

// Called every frame
void ACow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACow::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

