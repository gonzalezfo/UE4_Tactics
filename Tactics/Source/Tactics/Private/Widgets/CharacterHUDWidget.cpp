// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CharacterHUDWidget.h"

#include "Character/CustomCharacter.h"
#include "CameraPawn/CameraPawn.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

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
	if (!FinishTurnButton->OnClicked.Contains(this, "FinishTurnButtonClicked"))
	{
		FinishTurnButton->OnClicked.AddDynamic(this, &UCharacterHUDWidget::FinishTurnButtonClicked);
	}
}

void UCharacterHUDWidget::AttackButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("You are attacking"));
	//current_character_->TurnAvailable = false;
}

void UCharacterHUDWidget::DefenseButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("You are defending"));
	//current_character_->TurnAvailable = false;
}

void UCharacterHUDWidget::FinishTurnButtonClicked()
{
	current_character_->Unselected();
	current_character_->TurnAvailable = false;

	ACameraPawn* pawn = Cast<ACameraPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (pawn)
	{
		pawn->ResetSelection();
	}
}

void UCharacterHUDWidget::SetCharacterName(FString name)
{
	CharacterName->SetText(FText::FromString(name));
}
