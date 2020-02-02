// Fill out your copyright notice in the Description page of Project Settings.


#include "Cow.h"

#include "Player/FarmerPlayer.h"
#include "Player/FarmerController.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "CowAnimInstance.h"

// Sets default values
ACow::ACow()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HungryWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Hungry Widget"));
	HungryWidget->SetupAttachment(GetRootComponent());
	HungryWidget->RelativeLocation = FVector(90.0f, 0.0f, 100.0f);

	CowView = CreateDefaultSubobject<UCameraComponent>(TEXT("Cow View"));
	CowView->SetupAttachment(GetRootComponent());
	CowView->RelativeLocation = FVector(225, 0, 0.0f);
	CowView->RelativeRotation = FRotator(0, -180.0f, 0.0f);

	CowMinigameView = CreateDefaultSubobject<UCameraComponent>(TEXT("Cow Minigame View"));
	CowMinigameView->SetupAttachment(GetRootComponent());
	CowMinigameView->RelativeLocation = FVector(225, 20, 0.0f);
	CowMinigameView->RelativeRotation = FRotator(0, -140.0f, 0.0f);

	GetCharacterMovement()->MaxWalkSpeed = 150.0f;

}

// Called when the game starts or when spawned
void ACow::BeginPlay()
{
	Super::BeginPlay();
	
	CowDMI = GetMesh()->CreateDynamicMaterialInstance(0);
	if (GetMesh())
	{
		CowAnimInstance = Cast<UCowAnimInstance>(GetMesh()->GetAnimInstance());
	}
}

bool ACow::Interact_Implementation(AFarmerPlayer* InteractPlayer)
{
	if (InteractPlayer->CurrentHoldingHay)
	{
		InteractPlayer->RemoveHay();
		IncreaseHunger();
		return true;
	}

	return false;
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

void ACow::IncreaseHunger()
{
	fCowFullness += fFeedHungerIncrease;
	fCowFullness = FMath::Min(fCowFullness, 100.0f);	
}

void ACow::CowComplete()
{
	bGivenMilk = true;
	CowDMI->SetScalarParameterValue("Happy", 1.0f);
}

void ACow::PlayReaction(EReactionType ReactionType, float Intensity)
{
	switch (ReactionType)
	{
	case EReactionType::E_ANGRY:
		if (CowAnimInstance)
		{
			CowAnimInstance->PlayAngryAnimation(Intensity);
		}
		break;
	case EReactionType::E_SAD:
		if (CowAnimInstance)
		{
			CowAnimInstance->PlaySadAnimation(Intensity);
		}
		break;
	case EReactionType::E_HAPPY:
		if (CowAnimInstance)
		{
			CowAnimInstance->PlayHappyAnimation(Intensity);
		}
		break;
	case EReactionType::E_NEUTRAL:
		if (CowAnimInstance)
		{
			CowAnimInstance->PlayNeutralAnimation(Intensity);
		}
		break;
	default:
		break;
	}
}

// Called every frame
void ACow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FarmerControllerRef)
	{
		if (AFarmerController* FarmerController = Cast<AFarmerController>(GetWorld()->GetFirstPlayerController()))
		{
			FarmerControllerRef = FarmerController;
		}
	}

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (CameraManager && HungryWidget)
	{
		FRotator NewRotation = HungryWidget->GetComponentRotation();
		NewRotation.Yaw = CameraManager->GetCameraRotation().Yaw + 180.0f;
		HungryWidget->SetWorldRotation(NewRotation);
	}

	if (FarmerControllerRef)
	{
		// If interacting, dont decrease hunger
		if (FarmerControllerRef->bIsInteracting)
			return;
	}

	if (CurrentDelay <= 0)
	{
		fCowHungerRate += FMath::RandRange(fCowHungerRateChangeMin, fCowHungerRateChangeMax);
		fCowHungerRate = FMath::Clamp(fCowHungerRate, 0.0f, fCowHungerMaxRate);

		CurrentDelay = fChangeRateDelay;
	}
	CurrentDelay -= DeltaTime;

	fCowFullness -= (fCowHungerRate / fOverallHungerRateSlow) * DeltaTime;
	fCowFullness = FMath::Max(fCowFullness, 0.0f);

	if (fCowFullness <= 0.0f)
	{
		if (FarmerControllerRef)
			FarmerControllerRef->GameLost(CowName + " Died");
	}

}

// Called to bind functionality to input
void ACow::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

