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

	TeamNum = 255;

	movement_time_ = 0.0f;

	mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Character Mesh Component"));
	SetRootComponent(mesh_);
}

bool ACustomCharacter::IsFriendly(AActor* other)
{
	if (!other)
	{
		return false;
	}

	ACustomCharacter* otherChar = Cast<ACustomCharacter>(other);

	if (otherChar)
	{
		return (TeamNum == otherChar->TeamNum);
	}

	return false;
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

	MoveAlongPath(DeltaTime);
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

void ACustomCharacter::MoveAlongPath(float DeltaTime)
{
	if (path_cells_.Num() != 0)
	{
		ACell* tmp = path_cells_.Top();

		if (tmp)
		{
			movement_time_ += DeltaTime * movement_speed_;

			SetCell(tmp);

			if (movement_time_ >= 1.0f)
			{
				current_cell_ = tmp;
				tmp->SetCharacterPointer(this);

				path_cells_.Remove(tmp);
				movement_time_ = 0.0f;
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
	current_cell_->HighlightCell(CellMaterial::kCellMaterial_CurrentCell);
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
		FVector current_position = current_cell_->GetActorLocation();
		FVector new_position = new_cell->GetActorLocation();

		float alpha = movement_time_;

		alpha = FMath::Clamp(alpha, 0.0f, 1.0f);

		new_position.X = FMath::Lerp(current_position.X, new_position.X, alpha);
		new_position.Y = FMath::Lerp(current_position.Y, new_position.Y, alpha);
		new_position.Z += 50.0f;

		SetActorLocation(new_position);
	}
}

void ACustomCharacter::SetCharacterTeam(int team_value) {
	TeamNum = team_value;
}

int ACustomCharacter::GetCharacterTeam() {
	return TeamNum;
}

void ACustomCharacter::UpdateMaterial() {
	switch (TeamNum) {
	case kSpawnTeam_Player:
		if (CharacterMaterials[0] != nullptr) mesh_->SetMaterial(0, CharacterMaterials[0]);
		break;
	case kSpawnTeam_Team_1:
		if (CharacterMaterials[1] != nullptr) mesh_->SetMaterial(0, CharacterMaterials[1]);
		break;
	case kSpawnTeam_Team_2:
		if (CharacterMaterials[2] != nullptr) mesh_->SetMaterial(0, CharacterMaterials[2]);
		break;
	case kSpawnTeam_Team_3:
		if (CharacterMaterials[3] != nullptr) mesh_->SetMaterial(0, CharacterMaterials[3]);
		break;
	}
}
