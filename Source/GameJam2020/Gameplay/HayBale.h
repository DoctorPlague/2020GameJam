// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractInterface.h"
#include "HayBale.generated.h"

UCLASS()
class GAMEJAM2020_API AHayBale : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHayBale();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		bool Interact(class AFarmerPlayer* InteractPlayer);
	virtual bool Interact_Implementation(class AFarmerPlayer* InteractPlayer) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
		class UAnimMontage* GrabHayMontage;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> HayPiece;
};
