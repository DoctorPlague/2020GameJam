// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CowAIController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEJAM2020_API ACowAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	// Constructor
	ACowAIController();

	// Overrides
	void Tick(float DeltaTime) override;
	void OnPossess(APawn* InPawn) override;
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

	// Custom functions
	UFUNCTION(BlueprintImplementableEvent)
	FVector GetWanderLocation(bool& result);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ACow* Cow;

protected:

	float TimeSpentWandering;
	float WanderCooldownTimer;
	float WanderCooldownLength;
	bool IsWandering;
};
