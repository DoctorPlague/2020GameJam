// Fill out your copyright notice in the Description page of Project Settings.

#include "CowAIController.h"

#include "Gameplay/Cow.h"


ACowAIController::ACowAIController()
{

}

void ACowAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Cow && Cow->bCanMove && !IsWandering)
	{
		// increment
		WanderCooldownTimer += DeltaTime;

		// Wander activation
		if (WanderCooldownTimer >= WanderCooldownLength)
		{
			UE_LOG(LogTemp, Warning, TEXT("Attempting wander"))

			bool IsSuccessful = false;
			FVector Location = GetWanderLocation(IsSuccessful);
			if (IsSuccessful)
			{
				UE_LOG(LogTemp, Warning, TEXT("GetWanderLocation succeeded"))
				FAIMoveRequest MoveRequest;
				MoveRequest.SetGoalLocation(Location);
				MoveRequest.SetAcceptanceRadius(50.0f);
				FPathFollowingRequestResult MoveToResult = MoveTo(MoveRequest);
				IsWandering = true;

				//switch (MoveToResult.Code.GetValue())
				//{
				//case EPathFollowingRequestResult::AlreadyAtGoal:
				//{
				//	UE_LOG(LogTemp, Warning, TEXT("AlreadyAtGoal"))
				//	break;
				//}
				//case EPathFollowingRequestResult::Failed:
				//{
				//	UE_LOG(LogTemp, Warning, TEXT("Failed"))
				//	break;
				//}
				//case EPathFollowingRequestResult::RequestSuccessful:
				//{
				//	UE_LOG(LogTemp, Warning, TEXT("RequestSuccessful"))
				//	break;
				//}
				//}				
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("GetWanderLocation failed"))
				//..
			}			
		}
	}
	else if (IsWandering)
	{
		// If we're stuck wandering for too long, unstuck
		TimeSpentWandering += DeltaTime;
		if (TimeSpentWandering >= 10.0f)
		{
			StopMovement();
			IsWandering = false;
			WanderCooldownTimer = 0.0f;
		}
	}
}

void ACowAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Warning, TEXT("On Possess"))
	
	Cow = Cast<ACow>(InPawn);
	WanderCooldownLength = 3.0f;
	WanderCooldownTimer = 3.0f;
	TimeSpentWandering = 0.0f;
	IsWandering = false;
}

void ACowAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	IsWandering = false;
	WanderCooldownTimer = 0.0f;
	TimeSpentWandering = 0.0f;
}
