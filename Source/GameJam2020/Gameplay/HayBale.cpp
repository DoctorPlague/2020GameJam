// Fill out your copyright notice in the Description page of Project Settings.


#include "HayBale.h"

#include "Player/FarmerPlayer.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

// Sets default values
AHayBale::AHayBale()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHayBale::BeginPlay()
{
	Super::BeginPlay();
	
}

bool AHayBale::Interact_Implementation(class AFarmerPlayer* InteractPlayer)
{
	if (InteractPlayer)
	{
		if (UAnimInstance* AnimInstance = InteractPlayer->GetMesh()->GetAnimInstance())
		{
			if (GrabHayMontage)
				AnimInstance->Montage_Play(GrabHayMontage);
		}
	}
	if (!HayPiece)
		return false;

	AActor* SpawnedHay = GetWorld()->SpawnActor(HayPiece);
	if (SpawnedHay)
		InteractPlayer->PickedUpHay(SpawnedHay);

	return true;
}

// Called every frame
void AHayBale::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

