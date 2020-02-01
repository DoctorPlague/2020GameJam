// Fill out your copyright notice in the Description page of Project Settings.


#include "FarmerController.h"

#include "Gameplay/Cow.h"
#include "Player/FarmerPlayer.h"

#include "Camera/CameraActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include "Engine.h"

void AFarmerController::InteractWithCow(class ACow* _Cow)
{
	if (!FarmerPlayerRef)
		return;
	CurrentCow = _Cow;
	FVector CowToPlayerDir = (FarmerPlayerRef->GetActorLocation() - _Cow->GetActorLocation()).GetSafeNormal();

	FVector CowSideVector = _Cow->GetActorRightVector();
	float Dot = FVector::DotProduct(CowToPlayerDir, _Cow->GetActorRightVector());
	if (Dot > 0)
	{
		CowSideVector *= -1.0f;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Dot product: " + FString::SanitizeFloat(Dot));

	FVector NewCamDir = FMath::Lerp(CowSideVector, CowToPlayerDir, 0.6f);

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
	bShowMouseCursor = true;
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

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
	FInputActionBinding& InteractCompleteBind = InputComponent->BindAction("CompleteInteract", IE_Released, this, &AFarmerController::TempSuccessInteract);
	InteractCompleteBind.bConsumeInput = false;
	InputComponent->BindAction("FailInteract", IE_Released, this, &AFarmerController::TempFailInteract);
}

void AFarmerController::TempSuccessInteract()
{
	if (CurrentCow)
		AddSuccessfulCow(CurrentCow);

	CompleteInteract();
}

void AFarmerController::TempFailInteract()
{
	if (CurrentCow)
		CurrentCow->DecreaseRandHunger();

	CompleteInteract();
}

void AFarmerController::CompleteInteract()
{
	if (!bIsInteracting)
		return;
	if (FarmerPlayerRef)
		SetViewTargetWithBlend(FarmerPlayerRef, fInteractReturnToPlayerTime);

	bIsInteracting = false;
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly::FInputModeGameOnly());


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
