// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CharacterHUDWidget.h"

#include "Grid/Cell.h"

#include "Character/CustomCharacter.h"
#include "CameraPawn/CameraPawn.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UCharacterHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!MoveButton->OnClicked.Contains(this, "MoveButtonClicked"))
	{
		MoveButton->OnClicked.AddDynamic(this, &UCharacterHUDWidget::MoveButtonClicked);
	}
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

	selected_action_ = kSelectedAction_None;
}

void UCharacterHUDWidget::AttackButtonClicked()
{
	selected_action_ = kSelectedAction_Attacking;
	//current_character_->TurnAvailable = false;
}

void UCharacterHUDWidget::MoveButtonClicked()
{
	selected_action_ = kSelectedAction_Moving;

	ACameraPawn* pawn = Cast<ACameraPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (pawn)
	{
		ACustomCharacter* character_ = pawn->GetCharacter();

		if (character_)
		{
			ACell* cell_ = character_->GetCell();
			if (cell_)
			{
				cell_->GetGridPointer()->HighlightCells(character_->GetMovableCells());
				cell_->HighlightCell(CellMaterial::kCellMaterial_CurrentCell);
			}
		}

	}
	//current_character_->TurnAvailable = false;
}

void UCharacterHUDWidget::DefenseButtonClicked()
{
	selected_action_ = kSelectedAction_Defending;
	
	ACameraPawn* pawn = Cast<ACameraPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (pawn)
	{
		pawn->Defend();
		pawn->EndTurn();
	}
}

void UCharacterHUDWidget::FinishTurnButtonClicked()
{
	selected_action_ = kSelectedAction_EndTurn;

	ACameraPawn* pawn = Cast<ACameraPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (pawn)
	{
		pawn->EndTurn();
	}
}

void UCharacterHUDWidget::SetCharacterName(FString name)
{
	CharacterName->SetText(FText::FromString(name));
}
