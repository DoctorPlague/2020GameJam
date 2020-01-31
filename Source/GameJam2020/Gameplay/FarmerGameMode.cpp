// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FarmerGameMode.h"
#include "FarmerPlayer.h"
#include "UObject/ConstructorHelpers.h"

AFarmerGameMode::AFarmerGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Player/FarmerPlayerBP"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
