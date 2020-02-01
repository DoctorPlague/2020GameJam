// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FarmerPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Gameplay/Cow.h"
#include "FarmerController.h"

#include "InteractInterface.h"

//////////////////////////////////////////////////////////////////////////
// AFarmerPlayer

AFarmerPlayer::AFarmerPlayer()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}


//////////////////////////////////////////////////////////////////////////
// Input

void AFarmerPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AFarmerPlayer::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AFarmerPlayer::StopSprint);

	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AFarmerPlayer::Interact);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFarmerPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFarmerPlayer::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFarmerPlayer::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFarmerPlayer::LookUpAtRate);

}
void AFarmerPlayer::StartSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = fSprintSpeed;
}
void AFarmerPlayer::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = fWalkSpeed;
}

void AFarmerPlayer::Interact()
{
	FHitResult InteractHit;
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * fInteractDistance;
	if (GetWorld()->LineTraceSingleByChannel(InteractHit, Start, End, ECC_GameTraceChannel1))
	{
		if (InteractHit.Actor->Implements<UInteractInterface>())
		{
			if (IInteractInterface::Execute_Interact(InteractHit.Actor.Get(), this))
			{
				return;
			}
		}

	}

	if (!FarmerControllerRef)
		return;


	ACow* ClosestCow = GetClosestCow();
	if (!ClosestCow)
		return;

	FarmerControllerRef->InteractWithCow(ClosestCow);
	DisableInput(FarmerControllerRef);
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ClosestCow->GetActorLocation()).Yaw;
	SetActorRotation(NewRotation);
}
ACow* AFarmerPlayer::GetClosestCow()
{
	// No cows
	if (CurrentCowsInRange.Num() <= 0)
		return nullptr;

	int iClosestIndex = 0;
	for (int i = 0; i < CurrentCowsInRange.Num(); i++)
	{
		float DistToClosest = (GetActorLocation() - CurrentCowsInRange[iClosestIndex]->GetActorLocation()).Size();
		float DistToCurrent = (GetActorLocation() - CurrentCowsInRange[i]->GetActorLocation()).Size();
		if (DistToCurrent < DistToClosest)
		{
			iClosestIndex = i;
		}
	}

	return CurrentCowsInRange[iClosestIndex];
}

void AFarmerPlayer::PickedUpHay(AActor* Hay)
{
	CurrentHoldingHay = Hay;
	BI_OnPickedUpHay();
}

void AFarmerPlayer::RemoveHay()
{
	if (CurrentHoldingHay)
		CurrentHoldingHay->Destroy();

	CurrentHoldingHay = nullptr;
}

void AFarmerPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AFarmerController* FarmerController = Cast<AFarmerController>(NewController))
	{
		FarmerControllerRef = FarmerController;
	}
}

void AFarmerPlayer::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFarmerPlayer::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AFarmerPlayer::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AFarmerPlayer::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}