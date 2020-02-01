// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnumsStructs.h"
#include "FarmerController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEJAM2020_API AFarmerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void InteractWithCow(class ACow* _Cow);

	UFUNCTION(BlueprintCallable)
	void ShowDialogue(const FString& _Message);
	UFUNCTION(BlueprintCallable)
	void SpeechGame(EReactionType _ExpectedReaction);

	void Continue();
	void ChangeToDialogue();
	void ChangeToSpeechGame();

	UFUNCTION(BlueprintImplementableEvent)
		void BI_OnStartConversationView();
	UFUNCTION(BlueprintImplementableEvent)
		void BI_OnStartCowView();
	UFUNCTION(BlueprintImplementableEvent)
		void BI_OnConversationView();
	UFUNCTION(BlueprintImplementableEvent)
		void BI_OnCowView();

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

	void StartCompleteInteract();
	void CompleteInteract();

	UFUNCTION(BlueprintCallable)
	void SuccededInteract();
	UFUNCTION(BlueprintCallable)
	void FailedInteract();

	UFUNCTION()
	void AddSuccessfulCow(class ACow* _Cow);
public:
	class AFarmerPlayer* FarmerPlayerRef;

	UPROPERTY(BlueprintReadWrite)
	class ACameraActor* ConversationCamera;

	UPROPERTY(BlueprintReadWrite)
	bool bIsInteracting = false;

	UPROPERTY(BlueprintReadWrite)
	TArray<ACow*> CowsComplete;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int iNumberOfCowsNeeded = 5;
protected:
	class ACow* CurrentCow;

	UPROPERTY(BlueprintReadWrite)
	bool bCowView = false;

	UPROPERTY(EditDefaultsOnly)
		float fInteractReturnToPlayerTime = 0.5f;

	UPROPERTY(EditDefaultsOnly)
		float fChangeToCowViewTime = 0.7f;
	UPROPERTY(EditDefaultsOnly)
		float fChangeToConverstationViewTime = 1.0f;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UDialogueWidget> DialogueWidgetClass;

	UDialogueWidget* CurrentWidget;
};
