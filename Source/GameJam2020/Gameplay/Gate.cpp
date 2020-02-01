// Fill out your copyright notice in the Description page of Project Settings.


#include "Gate.h"

// Sets default values
AGate::AGate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGate::BeginPlay()
{
	Super::BeginPlay();
	
	NewRotation = GetActorRotation();
	InitialYaw = NewRotation.Yaw;
}

void AGate::HandleRotation(float DeltaTime)
{
	if (NewRotation.Yaw == GetActorRotation().Yaw)
		return;

	FRotator CurrentRotation = FMath::Lerp(GetActorRotation(), NewRotation, DeltaTime * fRotateSpeed);
	SetActorRotation(CurrentRotation);
	if (abs(NewRotation.Yaw - GetActorRotation().Yaw) <= 1.0f)
	{
		SetActorRotation(NewRotation);
		if (NewRotation.Yaw == InitialYaw)
			bOpened = false;
		else
			bOpened = true;
	}
}

// Called every frame
void AGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleRotation(DeltaTime);
}

bool AGate::Interact_Implementation(class AFarmerPlayer* InteractPlayer)
{
	if (!bOpened)
		NewRotation.Yaw = InitialYaw + 90.0f;
	else
		NewRotation.Yaw = InitialYaw;

	return true;
}

