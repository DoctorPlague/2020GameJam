// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CowAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GAMEJAM2020_API UCowAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	void NativeUpdateAnimation(float DeltaSeconds);
	void NativeBeginPlay();	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RunThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsWalking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsRunning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsIdle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ACow* Cow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UPawnMovementComponent* MovementComponent;
};
