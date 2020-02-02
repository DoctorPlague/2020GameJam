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
	bCowView = true; // reverses so starts dialogue

	//ShowDialogue("Moo moo");
	ChangeToDialogue();

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

	SetViewTargetWithBlend(ConversationCamera, fChangeToConverstationViewTime);

	BI_OnStartConversationView();
}

void AFarmerController::ChangeViewToCurrentCow()
{
	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
	}

	if (CurrentCow)
		SetViewTargetWithBlend(CurrentCow, fChangeToCowViewTime);

	bCowView = true;

	BI_OnStartCowView();
}

void AFarmerController::ShowDialogue(const FString& _Message)
{

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

	BI_OnConversationView();
}

void AFarmerController::SpeechGame(EReactionType _ExpectedReaction)
{
	//ChangeViewToCurrentCow();

	BI_OnCowView();
}

void AFarmerController::Continue()
{
	if (!CurrentCow)
		return;

	if (bCowView)
	{
		ChangeToDialogue();
	}
	else
	{
		ChangeToSpeechGame();
	}
}

void AFarmerController::ChangeToDialogue()
{
	ChangeViewToConversation();

	FTimerHandle ChangeView;
	FTimerDelegate DialogueDelegate;
	DialogueDelegate.BindUFunction(this, FName("ShowDialogue"), FString("MOOO moo"));
	GetWorldTimerManager().SetTimer(ChangeView, DialogueDelegate, fChangeToConverstationViewTime, false);
}

void AFarmerController::ChangeToSpeechGame()
{
	ChangeViewToCurrentCow();

	FTimerHandle ChangeView;
	FTimerDelegate SpeechGameDelegate;
	SpeechGameDelegate.BindUFunction(this, FName("SpeechGame"), EReactionType::E_AGRESSIVE);
	GetWorldTimerManager().SetTimer(ChangeView, SpeechGameDelegate, fChangeToCowViewTime, false);
}


void AFarmerController::ShowEndScreen(FString _Status, FString _Reason)
{
	if (FarmerPlayerRef)
		FarmerPlayerRef->DisableInput(this);

	BI_OnShowEndScreen(_Status, _Reason);
}

void AFarmerController::GameLost(FString _Reason)
{
	ShowEndScreen("Game Lost", _Reason);
	BI_OnGameLost();
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
	StartCompleteInteract();

	if (CurrentCow)
	{
		//AddSuccessfulCow(CurrentCow);
		FTimerHandle ReturnControl;
		FTimerDelegate SuccessDelegate;
		SuccessDelegate.BindUFunction(this, FName("AddSuccessfulCow"), CurrentCow);
		GetWorldTimerManager().SetTimer(ReturnControl, SuccessDelegate, fInteractReturnToPlayerTime, false);
	}
}

void AFarmerController::FailedInteract()
{
	if (CurrentCow)
		CurrentCow->DecreaseRandHunger();

	StartCompleteInteract();
}

void AFarmerController::StartCompleteInteract()
{
	if (FarmerPlayerRef)
		SetViewTargetWithBlend(FarmerPlayerRef, fInteractReturnToPlayerTime);

	FTimerHandle ReturnControl;
	GetWorldTimerManager().SetTimer(ReturnControl, this, &AFarmerController::CompleteInteract, fInteractReturnToPlayerTime);

	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
	}
	
	BI_OnStartInteractComplete();
}

void AFarmerController::CompleteInteract()
{
	if (!bIsInteracting)
		return;

	bIsInteracting = false;
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly::FInputModeGameOnly());

	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
	}

	if (CurrentCow)
	{
		CurrentCow->SetMovementEnabled(true);
		CurrentCow = nullptr;
	}
	

	if (FarmerPlayerRef)
		FarmerPlayerRef->EnableInput(this);
	BI_OnInteractComplete();
}




void AFarmerController::AddSuccessfulCow(ACow* _Cow)
{
	CowsComplete.Add(_Cow);
	_Cow->CowComplete();

	float PercentageComplete = float(CowsComplete.Num()) / float(iNumberOfCowsNeeded);

	BI_OnAddSuccessfulCow(PercentageComplete);

	if (PercentageComplete >= 1.0f)
	{
		ShowEndScreen("Game Complete!", "All Cows are Happy!");
		// GAME COMPLETE
	}
}
