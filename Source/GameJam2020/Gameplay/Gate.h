// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractInterface.h"
#include "Gate.generated.h"

UCLASS()
class GAMEJAM2020_API AGate : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void HandleRotation(float DeltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		bool Interact(class AFarmerPlayer* InteractPlayer);
	virtual bool Interact_Implementation(class AFarmerPlayer* InteractPlayer) override;

public:

protected:

	FRotator NewRotation;
	float InitialYaw;

	bool bOpened = false;

	UPROPERTY(EditDefaultsOnly)
		float fRotateSpeed = 50.0f;
};
