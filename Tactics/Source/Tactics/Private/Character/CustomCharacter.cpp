// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CustomCharacter.h"

#include "Grid/Cell.h"
#include "CameraPawn/CameraPawn.h"
#include "Widgets/CharacterHUDWidget.h"
#include "Components/HealthComponent.h"
#include "Core/CustomGameMode.h"

#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"


// Sets default values
ACustomCharacter::ACustomCharacter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TeamNum = 255;
	bDied = false;
	isDefending = false;
	movement_time_ = 0.0f;

	mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Character Mesh Component"));
	SetRootComponent(mesh_);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(mesh_);

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
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

	//Adds the HUD Widget to Viewport.
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UCharacterHUDWidget>(GetWorld(), HUDWidgetClass);

		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
			HUDWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	HealthComp->OnHealthChanged.AddDynamic(this, &ACustomCharacter::OnHealthChanged);

	//Sets the character state.
	state_ = CharacterState::kCharacterState_Idle;
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
				if (target->IsWalkable() && target->GetCharacterPointer() == nullptr) {
					AddCells(cell_array, target, range - 1);
				}
			}

			//south
			target_id = grid->South(start);
			target = grid->GetCellByID(target_id);
			if (target) {
				if (target->IsWalkable() && target->GetCharacterPointer() == nullptr) {
					AddCells(cell_array, target, range - 1);
				}
			}

			//east
			target_id = grid->East(start);
			target = grid->GetCellByID(target_id);
			if (target) {
				if (target->IsWalkable() && target->GetCharacterPointer() == nullptr) {
					AddCells(cell_array, target, range - 1);
				}
			}

			//west
			target_id = grid->West(start);
			target = grid->GetCellByID(target_id);
			if (target) {
				if (target->IsWalkable() && target->GetCharacterPointer() == nullptr) {
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
		state_ = CharacterState::kCharacterState_Moving;

		ACell* tmp = path_cells_.Top();

		if (tmp)
		{
			movement_time_ += DeltaTime * movement_speed_;

			SetCell(tmp);

			if (movement_time_ >= 1.0f)
			{
				current_cell_->SetCharacterPointer(nullptr);
				current_cell_ = tmp;
				tmp->SetCharacterPointer(this);

				path_cells_.Remove(tmp);
				cells_moved_this_turn_++;
				movement_time_ = 0.0f;

				//Checks if the array is empty which means the character finished its movement.
				if (path_cells_.Num() == 0)
				{
					if (camera_pawn_)
					{
						state_ = CharacterState::kCharacterState_FinishMovement;
					}
				}
			}
		}
	}
}

void ACustomCharacter::Defend()
{
	isDefending = true;
}

void ACustomCharacter::EndTurn()
{
	TurnAvailable = false;

	HUDWidget->SetVisibility(ESlateVisibility::Hidden);

	//Sets the view target to the camera pawn.
	GetWorldTimerManager().SetTimer(handle_, this, &ACustomCharacter::ReturnToMainCamera, 0.3f, false);
}

void ACustomCharacter::StartTurn()
{
	TurnAvailable = true;
	isDefending = false;
	cells_moved_this_turn_ = 0;
}


void ACustomCharacter::ReturnToMainCamera()
{
	GetWorldTimerManager().ClearTimer(handle_);
	state_ = CharacterState::kCharacterState_Idle;
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->SetViewTargetWithBlend(camera_pawn_, 1.0f);
	}
}


TArray<ACell*> ACustomCharacter::GetMovableCells()
{
	TArray<ACell*> cells;
	if (current_cell_) {
		cells.AddUnique(current_cell_);

		AGrid* grid = current_cell_->GetGridPointer();
		if (grid) {
			AddCells(cells, current_cell_, range_ - cells_moved_this_turn_);
		}
	}

	return cells;
}

TArray<ACell*> ACustomCharacter::GetAttackCells()
{
	TArray<ACell*> cells;
	
	AGrid* grid = current_cell_->GetGridPointer();

	if (grid)
	{
		cells.Add(grid->Cells[grid->North(current_cell_)]);
		cells.Add(grid->Cells[grid->East(current_cell_)]);
		cells.Add(grid->Cells[grid->South(current_cell_)]);
		cells.Add(grid->Cells[grid->West(current_cell_)]);
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
	//current_cell_->GetGridPointer()->HighlightCells(GetSelectableCells());
	//current_cell_->HighlightCell(CellMaterial::kCellMaterial_CurrentCell);
}

void ACustomCharacter::Unselected()
{
	//Unhighlights the action cells.
	current_cell_->GetGridPointer()->UnhighlightCells(GetMovableCells());
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

void ACustomCharacter::SetCameraPointer(ACameraPawn* new_camera)
{
	camera_pawn_ = new_camera;
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

void ACustomCharacter::OnHealthChanged(UHealthComponent * OwningHealthComp, float CurrentHealth, float HealthDelta, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (CurrentHealth <= 0.0f && !bDied)
	{
		bDied = true;

		ACustomGameMode* GM = Cast<ACustomGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

		if (GM)
		{
			//DEFEAT CONDITION (assuming player is team 0)
			bool defeat = true;

			for (int i = 0; i < GM->GameTeams[0].TeamMembers.Num(); ++i)
			{
				if (!GM->GameTeams[0].TeamMembers[i]->bDied) {
					defeat = false;
					break;
				}
			}

			if (defeat)
			{
				//DEFEAT
				UE_LOG(LogTemp, Warning, TEXT("GAME OVER"));
				return;
			}

			//VICTORY CONDITION (assuming player is team 0)
			bool victory = true;

			for (int i = 1; i < GM->GameTeams.Num() && victory; ++i)
			{
				for (int j = 0; j < GM->GameTeams[i].TeamMembers.Num(); ++j)
				{
					if (!GM->GameTeams[i].TeamMembers[j]->bDied) {
						victory = false;
						break;
					}
				}
			}

			if(victory)
			{
				//VICTORY
				UE_LOG(LogTemp, Warning, TEXT("VICTORY"));
				return;
			}
		}

		//DetachFromControllerPendingDestroy();

		//SetLifeSpan(5.0f);
	}
}
