// Fill out your copyright notice in the Description page of Project Settings.


#include "FarmerController.h"

#include "Gameplay/Cow.h"
#include "Player/FarmerPlayer.h"

#include "Camera/CameraActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"

void AFarmerController::InteractWithCow(class ACow* _Cow)
{
	if (!FarmerPlayerRef)
		return;
	CurrentCow = _Cow;
	FVector CowToPlayerDir = (FarmerPlayerRef->GetActorLocation() - _Cow->GetActorLocation()).GetSafeNormal();

	FVector NewCamDir = FMath::Lerp(_Cow->GetActorRightVector(), CowToPlayerDir, 0.6f);

	FVector NewLocation = _Cow->GetActorLocation() + NewCamDir * 500.0f;
	NewLocation.Z += 100.0f;

	if (!ConversationCamera)
		return;
	ConversationCamera->SetActorLocation(NewLocation);

	float HalfLength = (FarmerPlayerRef->GetActorLocation() - _Cow->GetActorLocation()).Size();
	HalfLength /= 2.0f;
	FVector AimLoc = _Cow->GetActorLocation() + CowToPlayerDir * HalfLength;
	ConversationCamera->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(ConversationCamera->GetActorLocation(), AimLoc));

	SetViewTargetWithBlend(ConversationCamera, 1.0f);

	bIsInteracting = true;

	CurrentCow->SetMovementEnabled(false);

	BI_OnInteractWithCow(_Cow);
}

void AFarmerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (AFarmerPlayer* FarmerPlayer = Cast<AFarmerPlayer>(InPawn))
	{
		FarmerPlayerRef = FarmerPlayer;
	}
}

void AFarmerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	FInputActionBinding& InteractCompleteBind = InputComponent->BindAction("CompleteInteract", IE_Released, this, &AFarmerController::TempCompleteInteract);
	InteractCompleteBind.bConsumeInput = false;
}

void AFarmerController::TempCompleteInteract()
{
	if (!bIsInteracting)
		return;
	if (FarmerPlayerRef)
		SetViewTargetWithBlend(FarmerPlayerRef, fInteractReturnToPlayerTime);

	bIsInteracting = false;

	if (CurrentCow)
		AddSuccessfulCow(CurrentCow);

	CurrentCow->SetMovementEnabled(true);
	CurrentCow = nullptr;

	FTimerHandle ReturnControl;
	GetWorldTimerManager().SetTimer(ReturnControl, this, &AFarmerController::ResumePlayerInput, fInteractReturnToPlayerTime);
}

void AFarmerController::ResumePlayerInput()
{
	if (FarmerPlayerRef)
		FarmerPlayerRef->EnableInput(this);
}

void AFarmerController::AddSuccessfulCow(ACow* _Cow)
{
	CowsComplete.Add(_Cow);

	float PercentageComplete = float(CowsComplete.Num()) / float(iNumberOfCowsNeeded);

	BI_OnAddSuccessfulCow(PercentageComplete);

	if (PercentageComplete > 1.0f)
	{
		// GAME COMPLETE
	}
}
