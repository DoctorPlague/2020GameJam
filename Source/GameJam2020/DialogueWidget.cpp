// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"

void UDialogueWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	InputArray.Add("Moo? Moo moomoo moo moo!? Moooooooooo..");
	InputArray.Add("Moo! MOOOOOO!!!!! ... Moo moo.. Moo???");
	InputArray.Add("Moooooooooooooooooooooooooo... Moo.");
	InputArray.Add("Moo!? Moo!? Moo moo moomoo moo. Moo moo.");
	InputArray.Add("Moomoo, moo moo moomoo moo? Moomooo mooo..");
	InputArray.Add("Moo moo moo moo mooo mooooo mooooo.. mooooooooooooooooooo..");
	InputArray.Add("Mooooooo? Moooooooo... Moo moo moomoo moo!");
	InputArray.Add("MOO! Moo.. moo? Moomoo: moo, moo, moomoo, moo..");
	InputArray.Add("Moo... moo moo?");
	InputArray.Add("Moo moo moomoo moomoo moo...");
	InputArray.Add("Moo moo moo moo? Moo moo moomoo moomoomoo, moo! Moomoomoo!!");
	InputArray.Add("Moo.. moo... moo... moooooooo... mooooooooooooooooooooooooooooooooooooo..");
	InputArray.Add("Moomoo.. moomoo... moomoomoo...");
	InputArray.Add("Moo? ... Moo? ... Mooo?");
	InputArray.Add("Moomoomoomoomoomoomoo... Moo moo?");
	InputArray.Add("Moo... moo... moo? Moo? Moo?");
	InputArray.Add("............ Moo");
	InputArray.Add(".......... Moo .......... Moo? .... ooo...");
	
	InputString = InputArray[FMath::RandRange(0, InputArray.Num() - 1)];
	CowName = "Bessy";
	FullDialogue = CowName + ": " + InputString;
	IsDialogueBoxPopulated = false;
	DialogueCharIndex = 0;
	TypingSpeedBase = 0.05f;
	TypingSpeed = 0.05f;
	TypingTimer = 0.0f; 
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
	if (InputString == "")
	{
		InputString = InputArray[FMath::RandRange(0, InputArray.Num() - 1)];
	}
	CowName = _CowName;
	FullDialogue = CowName + ": " + InputString;
	IsDialogueBoxPopulated = false;
	TypingTimer = 0.0f;
	DialogueCharIndex = 0;
	if (DialogueBox)
		DialogueBox->SetText(FText());
	PopulateDialogueBox();
}

bool UDialogueWidget::IsComplete()
{
	return IsDialogueBoxPopulated;
}

