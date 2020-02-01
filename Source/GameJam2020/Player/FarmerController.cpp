// Fill out your copyright notice in the Description page of Project Settings.


#include "FarmerController.h"

#include "Gameplay/Cow.h"
#include "Player/FarmerPlayer.h"
#include "DialogueWidget.h"

#include "Camera/CameraActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include "Engine.h"

void AFarmerController::InteractWithCow(class ACow* _Cow)
{
	CurrentCow = _Cow;

	ShowDialogue("Moo moo");

	bIsInteracting = true;
	bShowMouseCursor = true;
	//FInputModeUIOnly InputMode;
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	if (CurrentCow)
		CurrentCow->SetMovementEnabled(false);

}

void AFarmerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (AFarmerPlayer* FarmerPlayer = Cast<AFarmerPlayer>(InPawn))
	{
		FarmerPlayerRef = FarmerPlayer;
	}
}

void AFarmerController::ChangeViewToConversation()
{
	if (!FarmerPlayerRef)
		return;
	if (!ConversationCamera)
		return;
	if (!CurrentCow)
		return;

	bCowView = false;

	FVector CowToPlayerDir = (FarmerPlayerRef->GetActorLocation() - CurrentCow->GetActorLocation()).GetSafeNormal();
	CowToPlayerDir.Z = 0.0f;

	FVector CowSideVector = CurrentCow->GetActorRightVector();
	float Dot = FVector::DotProduct(CowToPlayerDir, CurrentCow->GetActorRightVector());
	if (Dot > 0)
	{
		CowSideVector *= -1.0f;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Dot product: " + FString::SanitizeFloat(Dot));

	FVector NewCamDir = FMath::Lerp(CowSideVector, CowToPlayerDir, 0.6f);

	FVector NewLocation = CurrentCow->GetActorLocation() + NewCamDir * 500.0f;
	NewLocation.Z += 100.0f;

	ConversationCamera->SetActorLocation(NewLocation);

	float HalfLength = (FarmerPlayerRef->GetActorLocation() - CurrentCow->GetActorLocation()).Size();
	HalfLength /= 2.0f;
	FVector AimLoc = CurrentCow->GetActorLocation() + CowToPlayerDir * HalfLength;
	ConversationCamera->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(ConversationCamera->GetActorLocation(), AimLoc));

	SetViewTargetWithBlend(ConversationCamera, 1.0f);

	BI_OnConversationView();
}

void AFarmerController::ChangeViewToCurrentCow()
{
	if (CurrentCow)
		SetViewTargetWithBlend(CurrentCow, 0.7f);

	bCowView = true;

	BI_OnCowView();
}

void AFarmerController::ShowDialogue(FString _Message)
{
	ChangeViewToConversation();

	if (!CurrentWidget && DialogueWidgetClass)
	{
		CurrentWidget = CreateWidget<UDialogueWidget>(this, DialogueWidgetClass);

	}

	if (!CurrentWidget)
		return;

	FString CowName = "Bessy";
	if (CurrentCow)
		CowName = CurrentCow->CowName;
	CurrentWidget->AddToViewport();
	CurrentWidget->SetupDialogue(CowName, _Message);
}

void AFarmerController::SpeechGame(EReactionType _ExpectedReaction)
{
	ChangeViewToCurrentCow();

	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
	}
}

void AFarmerController::Continue()
{
	if (!CurrentCow)
		return;

	if (bCowView)
		ShowDialogue("MOOO moo");
	else
		SpeechGame(EReactionType::E_AGRESSIVE);
}

void AFarmerController::GameLost(FString _Reason)
{
	if (FarmerPlayerRef)
		FarmerPlayerRef->DisableInput(this);
	BI_OnGameLost(_Reason);
}

void AFarmerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	FInputActionBinding& InteractCompleteBind = InputComponent->BindAction("CompleteInteract", IE_Released, this, &AFarmerController::SuccededInteract);
	InteractCompleteBind.bConsumeInput = false;
	FInputActionBinding& InteractBind = InputComponent->BindAction("Interact", IE_Released, this, &AFarmerController::Continue);
	InteractBind.bConsumeInput = false;
	InputComponent->BindAction("FailInteract", IE_Released, this, &AFarmerController::FailedInteract);
}

void AFarmerController::SuccededInteract()
{
	if (CurrentCow)
		AddSuccessfulCow(CurrentCow);

	CompleteInteract();
}

void AFarmerController::FailedInteract()
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

	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
	}

	CurrentCow->SetMovementEnabled(true);
	CurrentCow = nullptr;

	FTimerHandle ReturnControl;
	GetWorldTimerManager().SetTimer(ReturnControl, this, &AFarmerController::ResumePlayerInput, fInteractReturnToPlayerTime);
	BI_OnInteractComplete();
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
