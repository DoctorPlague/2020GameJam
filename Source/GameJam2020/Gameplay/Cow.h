// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Cow.generated.h"

UCLASS()
class GAMEJAM2020_API ACow : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACow();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UWidgetComponent* HungryWidget;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SetMovementEnabled(bool _bEnabled);

	void DecreaseRandHunger();

protected:

public:
	UPROPERTY(BlueprintReadWrite)
		bool bCanMove = true;

	UPROPERTY(BlueprintReadWrite)
		float fCowHungerRate = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float fChangeRateDelay = 1.0f;
	float CurrentDelay = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float fCowFullness = 100.0f;
	


	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float fOverallHungerRateSlow = 15.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float fCowHungerRateChangeMin = -5.0f;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float fCowHungerRateChangeMax = 5.0f;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float fCowHungerMaxRate = 25.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float fFailInteractLoseMin = 5.0f;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float fFailInteractLoseMax = 10.0f;

};
