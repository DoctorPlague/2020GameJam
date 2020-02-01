// Fill out your copyright notice in the Description page of Project Settings.


#include "Cow.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include"Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/World.h"

// Sets default values
ACow::ACow()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HungryWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Hungry Widget"));
	HungryWidget->SetupAttachment(GetRootComponent());
	HungryWidget->RelativeLocation = FVector(90.0f, 0.0f, 100.0f);
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

void ACow::DecreaseRandHunger()
{
	float NewHungerLoss = FMath::RandRange(fFailInteractLoseMin, fFailInteractLoseMax);

	fCowFullness -= NewHungerLoss;
	fCowFullness = FMath::Max(fCowFullness, 0.0f);
}

// Called every frame
void ACow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentDelay <= 0)
	{
		fCowHungerRate += FMath::RandRange(fCowHungerRateChangeMin, fCowHungerRateChangeMax);
		fCowHungerRate = FMath::Clamp(fCowHungerRate, 0.0f, fCowHungerMaxRate);

		CurrentDelay = fChangeRateDelay;
	}
	CurrentDelay -= DeltaTime;

	fCowFullness -= (fCowHungerRate / fOverallHungerRateSlow) * DeltaTime;
	fCowFullness = FMath::Max(fCowFullness, 0.0f);

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (CameraManager && HungryWidget)
	{
		FRotator NewRotation = HungryWidget->GetComponentRotation();
		NewRotation.Yaw = CameraManager->GetCameraRotation().Yaw + 180.0f;
		HungryWidget->SetWorldRotation(NewRotation);
	}
}

// Called to bind functionality to input
void ACow::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

