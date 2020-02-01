// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"

void UDialogueWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InputString = "Moo? Moo moo moo moo moo!? Moooooooooo..";
	CowName = "Bessy";
	FullDialogue = CowName + ": " + InputString;
	IsDialogueBoxPopulated = false;
	DialogueCharIndex = 0;
	TypingSpeedBase = 0.05f;
	TypingSpeed = 0.05f;
	TypingTimer = 0.0f; 
	PopulateDialogueBox();

}

void UDialogueWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void UDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UDialogueWidget::NativeDestruct()
{
	Super::NativeDestruct();

}

void UDialogueWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsDialogueBoxPopulated)
	{
		if (TypingTimer > TypingSpeed)
		{
			PopulateDialogueBox();
			TypingTimer = 0.0f;
			TypingSpeed = TypingSpeedBase * FMath::FRandRange(0.75f, 3.0f);
		}
		else
		{
			TypingTimer += InDeltaTime;
		}	
	}
}

void UDialogueWidget::PopulateDialogueBox()
{
	if (!DialogueBox)
		return;
	FString CurrentString = DialogueBox->GetText().ToString();
	if (CurrentString == FullDialogue)
	{
		IsDialogueBoxPopulated = true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Adding char"))
		if (FullDialogue.Len() > DialogueCharIndex)
		{
			CurrentString.AppendChar(FullDialogue[DialogueCharIndex]);
			DialogueBox->SetText(FText::FromString(CurrentString));
			DialogueCharIndex++;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("DialogueCharIndex is out of range.."))
		}
		
	}
}

void UDialogueWidget::SetupDialogue(FString _CowName, FString _InputString)
{
	InputString = _InputString;
	CowName = _CowName;
	FullDialogue = CowName + ": " + InputString;
	IsDialogueBoxPopulated = false;
	TypingTimer = 0.0f;
	DialogueCharIndex = 0;
	if (DialogueBox)
		DialogueBox->SetText(FText());
	PopulateDialogueBox();
}

