// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "DialogueWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMEJAM2020_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// UserWidget functions
	virtual void NativeOnInitialized();
	virtual void NativePreConstruct();
	virtual void NativeConstruct();
	virtual void NativeDestruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void PopulateDialogueBox();	

	bool IsDialogueBoxPopulated;
	int DialogueCharIndex;
	FString CowName;
	FString InputString;
	FString FullDialogue;

	float TypingSpeedBase;
	float TypingSpeed;
	float TypingTimer;

	TArray<FString> InputArray;
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* DialogueBox;

	UFUNCTION(BlueprintCallable)
	void SetupDialogue(FString CowName, FString InputString = "");
	UFUNCTION(BlueprintCallable)
		void SkipDialogueTyping();

	UFUNCTION(BlueprintPure)
		bool IsComplete();
};
