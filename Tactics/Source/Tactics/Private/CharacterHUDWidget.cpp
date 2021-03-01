// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterHUDWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UCharacterHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!AttackButton->OnClicked.Contains(this, "AttackButtonClicked"))
	{
		AttackButton->OnClicked.AddDynamic(this, &UCharacterHUDWidget::AttackButtonClicked);
	}
	if (!DefenseButton->OnClicked.Contains(this, "DefenseButtonClicked"))
	{
		DefenseButton->OnClicked.AddDynamic(this, &UCharacterHUDWidget::DefenseButtonClicked);
	}
}

void UCharacterHUDWidget::AttackButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("You are attacking"));
}

void UCharacterHUDWidget::DefenseButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("You are defending"));
}

void UCharacterHUDWidget::SetCharacterName(FString name)
{
	CharacterName->SetText(FText::FromString(name));
}
