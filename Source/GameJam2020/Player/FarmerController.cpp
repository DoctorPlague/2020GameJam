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

	TargetDestination = CurrentCow->GetActorLocation() + CurrentCow->GetActorForwardVector() * FTargetDistanceInFrontofCow;

	CurrentStage = ECurrentStage::E_INITIALDIALOGUE;

	bComplete = false;
	bIsInteracting = true;
	bShowMouseCursor = true;
	//FInputModeUIOnly InputMode;
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	ChangeToDialogue("");

	if (CurrentCow)
		CurrentCow->SetMovementEnabled(false);

	BI_OnInteractWithCow(CurrentCow);
}

void AFarmerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (AFarmerPlayer* FarmerPlayer = Cast<AFarmerPlayer>(InPawn))
	{
		FarmerPlayerRef = FarmerPlayer;
	}
}

void AFarmerController::Continue()
{
	if (!CurrentCow)
		return;

	switch (CurrentStage)
	{
	case ECurrentStage::E_INITIALDIALOGUE:

		if (bComplete)
		{
			if (bCowWon)
			{
				SuccededInteract();
			}
			else
			{
				FailedInteract();
			}
		}
		else
		{
			bComplete = false;
			ChangeToSpeechGame();
		}
		break;
	case ECurrentStage::E_MINIGAME:
		//ChangeToCowView();
		break;
	case ECurrentStage::E_COWREACTION:
		ChangeToPlayerView(CowReaction);
		break;
	case ECurrentStage::E_PLAYERRESPONSE:
		//ChangeToDialogue("Moo Moo mooooo mo!");
		break;
	default:
		break;
	}

}
// ########### CAMERA VIEW ###############

void AFarmerController::ChangeViewToConversation()
{
	//if (!FarmerPlayerRef)
	//	return;
	if (!ConversationCamera)
		return;
	if (!CurrentCow)
		return;

	CurrentStage = ECurrentStage::E_INITIALDIALOGUE;

	FVector CowToPlayerDir = (TargetDestination - CurrentCow->GetActorLocation()).GetSafeNormal();
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

	float HalfLength = (TargetDestination - CurrentCow->GetActorLocation()).Size();
	HalfLength /= 2.0f;
	FVector AimLoc = CurrentCow->GetActorLocation() + CowToPlayerDir * HalfLength;
	ConversationCamera->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(ConversationCamera->GetActorLocation(), AimLoc));

	SetViewTargetWithBlend(ConversationCamera, fChangeToConverstationViewTime);

	BI_OnStartConversationView();
}

void AFarmerController::ChangeViewToCurrentCow()
{
	if (IsValid(CurrentWidget))
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}

	if (CurrentCow)
	{
		CurrentCow->CowView->SetActive(true);
		CurrentCow->CowMinigameView->SetActive(false);
		SetViewTargetWithBlend(CurrentCow, fChangeToCowViewTime);

	}

	CurrentStage = ECurrentStage::E_COWREACTION;

	BI_OnStartCowView();
}

void AFarmerController::ChangeViewToCurrentCowSpeechGame()
{
	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}


	if (CurrentCow)
	{
		CurrentCow->CowMinigameView->SetActive(true);
		CurrentCow->CowView->SetActive(false);
		SetViewTargetWithBlend(CurrentCow, fChangeToCowViewTime);
	}
	
	CurrentStage = ECurrentStage::E_MINIGAME;

	BI_OnStartSpeechGameView();
}

void AFarmerController::ChangeViewToPlayer()
{
	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}

	if (FarmerPlayerRef)
	{
		FarmerPlayerRef->ResponseCamera->SetActive(true);
		FarmerPlayerRef->GetFollowCamera()->SetActive(false);
		SetViewTargetWithBlend(FarmerPlayerRef, fChangeToCowViewTime);
	}

	bComplete = true;
	CurrentStage = ECurrentStage::E_PLAYERRESPONSE;

	BI_OnStartPlayerView();
}

// ########### NEW SCREEN ###############
void AFarmerController::ShowDialogue(const FString& _Message)
{
	GetWorldTimerManager().ClearTimer(ChangeView);
	ShowCowDialogue(_Message);

	BI_OnConversationView();
}


void AFarmerController::SpeechGame(EReactionType _ExpectedReaction)
{
	GetWorldTimerManager().ClearTimer(ChangeView);
	BI_OnSpeechGameView();
}

void AFarmerController::CowExpression(EReactionType _NewExpression)
{
	GetWorldTimerManager().ClearTimer(ChangeView);

	FString Message = "Wow";
	switch (_NewExpression)
	{
	case EReactionType::E_ANGRY:
		Message = "MOOOOO!";
		break;
	case EReactionType::E_HAPPY:
		Message = "Mo Moooo Mo!";
		break;
	case EReactionType::E_SAD:
		Message = "Mooo... mo moo";
		break;
	case EReactionType::E_NEUTRAL:
		Message = "..... Moo";
		break;
	default:
		break;
	}
	ShowCowDialogue(Message);

	BI_OnCowView();
}

void AFarmerController::PlayerResponse(EReactionType _ExpectedReaction)
{
	bComplete = true;
	GetWorldTimerManager().ClearTimer(ChangeView);
	BI_OnPlayerView(_ExpectedReaction);
}

void AFarmerController::ShowCowDialogue(const FString& _Message)
{
	if (!CurrentWidget && DialogueWidgetClass)
	{
		CurrentWidget = CreateWidget<UDialogueWidget>(this, DialogueWidgetClass);

	}

	if (!IsValid(CurrentWidget))
		return;


	FString CowName = "Bessy";
	if (CurrentCow)
		CowName = CurrentCow->CowName;
	CurrentWidget->AddToViewport();
	CurrentWidget->SetupDialogue(CowName, _Message);
}

// ########### CHANGE TO ###############
void AFarmerController::ChangeToDialogue(const FString& Message)
{
	if (ChangeView.IsValid())
	{
		return;
	}

	ChangeViewToConversation();

	FTimerDelegate DialogueDelegate;
	DialogueDelegate.BindUFunction(this, FName("ShowDialogue"), Message);
	GetWorldTimerManager().SetTimer(ChangeView, DialogueDelegate, fChangeToConverstationViewTime, false);
}


void AFarmerController::ChangeToSpeechGame()
{
	if (CurrentWidget)
	{
		if (!CurrentWidget->IsComplete())
			return;
	}
	if (ChangeView.IsValid())
	{
		return;
	}

	ChangeViewToCurrentCowSpeechGame();

	FTimerDelegate SpeechGameDelegate;
	SpeechGameDelegate.BindUFunction(this, FName("SpeechGame"), EReactionType::E_ANGRY);
	GetWorldTimerManager().SetTimer(ChangeView, SpeechGameDelegate, fChangeToCowViewTime, false);
}

void AFarmerController::ChangeToCowView()
{
	if (ChangeView.IsValid())
	{
		return;
	}

	ChangeViewToCurrentCow();

	FTimerDelegate CowViewDelegate;
	CowViewDelegate.BindUFunction(this, FName("CowExpression"), CowReaction);
	GetWorldTimerManager().SetTimer(ChangeView, CowViewDelegate, fChangeToCowViewTime, false);
}

void AFarmerController::ChangeToPlayerView(EReactionType _ExpectedReaction)
{
	if (CurrentWidget)
	{
		if (!CurrentWidget->IsComplete())
			return;
	}
	if (ChangeView.IsValid())
	{
		return;
	}

	ChangeViewToPlayer();

	FTimerDelegate PlayerResponseDelegate;
	PlayerResponseDelegate.BindUFunction(this, FName("PlayerResponse"), _ExpectedReaction);
	GetWorldTimerManager().SetTimer(ChangeView, PlayerResponseDelegate, fChangeToCowViewTime, false);
}

void AFarmerController::OnMinigameCompleted(bool _Succeeded, EReactionType _ExpectedReaction)
{
	if (!_Succeeded)
	{
		FailedInteract();
		return;
	}
	CowReaction = _ExpectedReaction;
	ChangeToCowView();

}

void AFarmerController::SetCurrentCowReaction(EReactionType _ExpectedReaction, float _fIntensity)
{
	if (CurrentCow)
	{
		CurrentCow->PlayReaction(_ExpectedReaction, _fIntensity);
	}
}

void AFarmerController::CompletedReaction(EReactionType _ExpectedReaction, EReactionType _GivenReaction)
{
	FString CowResponse = "MOOO!? :(";
	if (_ExpectedReaction == _GivenReaction)
	{
		CowResponse = "Moooo :)";
		bCowWon = true;
	}
	ChangeToDialogue(CowResponse);
}


void AFarmerController::ShowEndScreen(FString _Status, FString _Reason)
{
	if (bGameComplete)
		return;

	bGameComplete = true;

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

	FInputActionBinding& InteractBind = InputComponent->BindAction("Interact", IE_Pressed, this, &AFarmerController::Continue);
	InteractBind.bConsumeInput = false;

	InputComponent->BindAction("Cancel", IE_Pressed, this, &AFarmerController::CancelInteract);

}

void AFarmerController::SuccededInteract()
{
	StartCompleteInteract();

	if (CurrentCow)
	{
		//AddSuccessfulCow(CurrentCow);
		FTimerDelegate SuccessDelegate;
		SuccessDelegate.BindUFunction(this, FName("AddSuccessfulCow"), CurrentCow);
		GetWorldTimerManager().SetTimer(ChangeView, SuccessDelegate, fInteractReturnToPlayerTime, false);
	}
}

void AFarmerController::FailedInteract()
{
	if (CurrentCow)
		CurrentCow->DecreaseRandHunger();

	StartCompleteInteract();
}

void AFarmerController::CancelInteract()
{


	StartCompleteInteract();
}

void AFarmerController::StartCompleteInteract()
{
	GetWorldTimerManager().ClearTimer(ChangeView);

	if (FarmerPlayerRef)
	{
		FarmerPlayerRef->ResponseCamera->SetActive(false);
		FarmerPlayerRef->GetFollowCamera()->SetActive(true);
		SetViewTargetWithBlend(FarmerPlayerRef, fInteractReturnToPlayerTime);
	}

	FTimerHandle ReturnControl;
	GetWorldTimerManager().SetTimer(ReturnControl, this, &AFarmerController::CompleteInteract, fInteractReturnToPlayerTime);

	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
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
		CurrentWidget = nullptr;
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
	GetWorldTimerManager().ClearTimer(ChangeView);

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
