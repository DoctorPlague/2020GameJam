// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnumsStructs.h"
#include "FarmerController.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ECurrentStage : uint8
{
	E_INITIALDIALOGUE 	UMETA(DisplayName = "Initial Dialogue"),
	E_MINIGAME 	UMETA(DisplayName = "Minigame"),
	E_COWREACTION 	UMETA(DisplayName = "Cow Reaction"),
	E_PLAYERRESPONSE 	UMETA(DisplayName = "Player Response"),
};
/**
 * 
 */
UCLASS()
class GAMEJAM2020_API AFarmerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void InteractWithCow(class ACow* _Cow);
	UFUNCTION(BlueprintImplementableEvent)
		void BI_OnInteractWithCow(class ACow* _Cow);

	UFUNCTION()
	void ShowDialogue(const FString& _Message);
	UFUNCTION()
	void SpeechGame(EReactionType _ExpectedReaction);
	UFUNCTION()
	void CowExpression(EExpressionType _NewExpression);
	UFUNCTION()
	void PlayerResponse(EReactionType _ExpectedReaction);

	UFUNCTION(BlueprintCallable)
	void ShowCowDialogue(const FString& _Message);

	UFUNCTION(BlueprintCallable)
	void Continue();
	UFUNCTION(BlueprintCallable)
	void ChangeToDialogue(const FString& Message = "Moo moo");
	UFUNCTION(BlueprintCallable)
	void ChangeToSpeechGame();
	UFUNCTION(BlueprintCallable)
	void ChangeToCowView();
	UFUNCTION(BlueprintCallable)
	void ChangeToPlayerView(EReactionType _ExpectedReaction);

	UFUNCTION(BlueprintCallable)
		void OnMinigameCompleted(bool _Succeeded, EReactionType _ExpectedReaction);
	UFUNCTION(BlueprintCallable)
		void SetCurrentCowReaction(EReactionType _ExpectedReaction, float _fIntensity);

	UFUNCTION(BlueprintCallable)
		void CompletedReaction(EReactionType _ExpectedReaction, EReactionType _GivenReaction);

	UFUNCTION(BlueprintImplementableEvent)
		void BI_OnStartConversationView();
	UFUNCTION(BlueprintImplementableEvent)
		void BI_OnStartSpeechGameView();
	UFUNCTION(BlueprintImplementableEvent)
		void BI_OnStartCowView();
	UFUNCTION(BlueprintImplementableEvent)
		void BI_OnStartPlayerView();

	UFUNCTION(BlueprintImplementableEvent)
		void BI_OnConversationView();
	UFUNCTION(BlueprintImplementableEvent)
		void BI_OnCowView();
	UFUNCTION(BlueprintImplementableEvent)
		void BI_OnSpeechGameView();
	UFUNCTION(BlueprintImplementableEvent)
		void BI_OnPlayerView(EReactionType _ExpectedReaction);

	UFUNCTION(BlueprintImplementableEvent)
	void BI_OnAddSuccessfulCow(float _PercentageComplete);

	UFUNCTION(BlueprintImplementableEvent)
		void BI_OnInteractComplete();
	UFUNCTION(BlueprintImplementableEvent)
		void BI_OnStartInteractComplete();

	UFUNCTION(BlueprintCallable)
	void ShowEndScreen(FString _Status, FString _Reason);
	UFUNCTION(BlueprintImplementableEvent)
		void BI_OnShowEndScreen(const FString& _Status, const FString& _Reason);

	UFUNCTION(BlueprintCallable)
	void GameLost(FString _Reason);
	UFUNCTION(BlueprintImplementableEvent)
	void BI_OnGameLost();

protected:
	// APawn interface
	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;

	void ChangeViewToConversation();
	void ChangeViewToCurrentCow();
	void ChangeViewToCurrentCowSpeechGame();
	void ChangeViewToPlayer();

	void StartCompleteInteract();
	void CompleteInteract();

	UFUNCTION(BlueprintCallable)
	void SuccededInteract();
	UFUNCTION(BlueprintCallable)
		void FailedInteract();
	UFUNCTION(BlueprintCallable)
		void CancelInteract();

	UFUNCTION()
	void AddSuccessfulCow(class ACow* _Cow);
public:
	UPROPERTY(BlueprintReadWrite)
	class AFarmerPlayer* FarmerPlayerRef;

	UPROPERTY(BlueprintReadWrite)
	class ACameraActor* ConversationCamera;

	UPROPERTY(BlueprintReadWrite)
	bool bIsInteracting = false;

	UPROPERTY(BlueprintReadWrite)
	TArray<ACow*> CowsComplete;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int iNumberOfCowsNeeded = 5;

	UPROPERTY(BlueprintReadWrite)
	ECurrentStage CurrentStage = ECurrentStage::E_INITIALDIALOGUE;
	UPROPERTY(BlueprintReadWrite)
	EReactionType CowReaction;

protected:
	FTimerHandle ChangeView;

	class ACow* CurrentCow;

	UPROPERTY(BlueprintReadWrite)
		bool bCowWon = false;
	UPROPERTY(BlueprintReadWrite)
		bool bComplete = false;

	UPROPERTY(EditDefaultsOnly)
		float fInteractReturnToPlayerTime = 0.5f;

	UPROPERTY(EditDefaultsOnly)
		float fChangeToCowViewTime = 0.7f;
	UPROPERTY(EditDefaultsOnly)
		float fChangeToConverstationViewTime = 1.0f;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UDialogueWidget> DialogueWidgetClass;

	UDialogueWidget* CurrentWidget;

	UPROPERTY(BlueprintReadWrite)
	bool bGameComplete = false;


	UPROPERTY(EditDefaultsOnly)
		float FTargetDistanceInFrontofCow = 300.0f;
	UPROPERTY(BlueprintReadWrite)
		FVector TargetDestination = FVector(0.0f);
};
