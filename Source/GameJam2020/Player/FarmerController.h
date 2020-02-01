// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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


	UFUNCTION(BlueprintImplementableEvent)
	void BI_OnAddSuccessfulCow(float _PercentageComplete);

protected:
	// APawn interface
	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;

	void TempCompleteInteract();
	void ResumePlayerInput();

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

	UPROPERTY(EditDefaultsOnly)
		float fInteractReturnToPlayerTime = 0.5f;
};
