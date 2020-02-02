// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FarmerPlayer.generated.h"

UCLASS(config=Game)
class AFarmerPlayer : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	AFarmerPlayer();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* ResponseCamera;

	UFUNCTION(BlueprintPure)
	class ACow* GetClosestCow();

	void PickedUpHay(AActor* Hay);
	UFUNCTION(BlueprintImplementableEvent)
		void BI_OnPickedUpHay();

	void RemoveHay();

protected:


	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void StartSprint();
	void StopSprint();

	void Interact();

	virtual void PossessedBy(AController* NewController);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float fSprintSpeed = 600.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float fWalkSpeed = 300.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float fInteractDistance = 150.0f;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	class AFarmerController* FarmerControllerRef;
public:

	UPROPERTY(BlueprintReadWrite)
	TArray<ACow*> CurrentCowsInRange;

	UPROPERTY(BlueprintReadWrite)
	AActor* CurrentHoldingHay;
};

