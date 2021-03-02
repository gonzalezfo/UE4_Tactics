// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacter.h"

#include "Components/StaticMeshComponent.h"

#include "Cell.h"
#include "CharacterHUDWidget.h"


// Sets default values
ACustomCharacter::ACustomCharacter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Character Mesh Component"));
	SetRootComponent(mesh_);
}

// Called when the game starts or when spawned
void ACustomCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UCharacterHUDWidget>(GetWorld(), HUDWidgetClass);

		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
			HUDWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

// Called every frame
void ACustomCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACustomCharacter::InitPlayer(ACell* tmp)
{
	current_cell_ = tmp;
}

static void AddCells(TArray<ACell*>& cell_array, ACell* start, int range) {
	if (start) {
		cell_array.AddUnique(start);

		AGrid* grid = start->GetGridPointer();

		ACell* target;
		int target_id;

		if (range > 0 && grid) {
			//north
			target_id = grid->North(start);
			target = grid->GetCellByID(target_id);
			if (target) {
				if (target->IsWalkable()) {
					AddCells(cell_array, target, range - 1);
				}
			}

			//south
			target_id = grid->South(start);
			target = grid->GetCellByID(target_id);
			if (target) {
				if (target->IsWalkable()) {
					AddCells(cell_array, target, range - 1);
				}
			}

			//east
			target_id = grid->East(start);
			target = grid->GetCellByID(target_id);
			if (target) {
				if (target->IsWalkable()) {
					AddCells(cell_array, target, range - 1);
				}
			}

			//west
			target_id = grid->West(start);
			target = grid->GetCellByID(target_id);
			if (target) {
				if (target->IsWalkable()) {
					AddCells(cell_array, target, range - 1);
				}
			}
		}
	}
	
}

TArray<ACell*> ACustomCharacter::GetSelectableCells()
{
	TArray<ACell*> cells;
	if (current_cell_) {
		cells.AddUnique(current_cell_);

		AGrid* grid = current_cell_->GetGridPointer();
		if (grid) {
			AddCells(cells, current_cell_, range_);
		}
	}

	return cells;
}

void ACustomCharacter::Selected()
{
	//Makes the HUD visible and sets the character name
	HUDWidget->current_character_ = this;
	HUDWidget->SetCharacterName(name_);
	HUDWidget->SetVisibility(ESlateVisibility::Visible);

	//Highlights the action cells.
	current_cell_->GetGridPointer()->HighlightCells(GetSelectableCells());
}

void ACustomCharacter::Unselected()
{
	//Hides the HUD widget
	HUDWidget->SetVisibility(ESlateVisibility::Hidden);

	//Unhighlights the action cells.
	current_cell_->GetGridPointer()->UnhighlightCells(GetSelectableCells());
}

ACell* ACustomCharacter::GetCell()
{
	return current_cell_;
}

void ACustomCharacter::SetCell(ACell* new_cell)
{
	if (new_cell)
	{
		current_cell_ = new_cell;

		FVector new_position = new_cell->GetActorLocation();
		new_position.Z += 50.0f;

		SetActorLocation(new_position);
	}
}

