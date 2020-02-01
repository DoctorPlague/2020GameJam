// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "EnumsStructs.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EReactionType : uint8
{
	E_AGRESSIVE 	UMETA(DisplayName = "Agressive"),
	E_PLEASANT 	UMETA(DisplayName = "Pleasant"),
};

/**
 * 
 */
UCLASS()
class GAMEJAM2020_API UEnumsStructs : public UUserDefinedStruct
{
	GENERATED_BODY()
	
};
