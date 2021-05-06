// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CustomCharacter.h"

#include "Grid/Cell.h"
#include "CameraPawn/CameraPawn.h"
#include "Widgets/CharacterHUDWidget.h"
#include "Components/HealthComponent.h"
#include "Components/FSMComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/Button.h"

#include "Core/CustomGameMode.h"
#include "Widgets/FloatingTextWidget.h"

#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"

#include "TimerManager.h"


// Sets default values
ACustomCharacter::ACustomCharacter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TeamNum = 255;
	bDied = false;
	isDefending = false;
	movement_time_ = 0.0f;

	mesh_ = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(mesh_);
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(mesh_);

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));

	FiniteStateMachineComponent = CreateDefaultSubobject<UFSMComponent>(TEXT("FSMComp"));

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	WidgetComp->SetupAttachment(CameraComp);
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

	PC = Cast<APlayerController>(GetController());

	HUDWidget = CreateWidget<UCharacterHUDWidget>(GetWorld(), HUDWidgetClass);

	if (HUDWidget)
	{
		HUDWidget->AddToViewport();
		HUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	HealthComp->OnHealthChanged.AddDynamic(this, &ACustomCharacter::OnHealthChanged);
	
	UFloatingTextWidget* FloatingText = Cast<UFloatingTextWidget>(this->WidgetComp->GetUserWidgetObject());
	if (FloatingText)
	{
		FloatingText->InitOverheadHUDWidget(this);
	}


	bHasAttackedThisTurn = false;
	bHasHealedThisTurn = false;
	bHasHealed = false;
	bIsMoving = false;

	//Sets the character state.
	state_ = CharacterState::kCharacterState_Idle;
	mesh_->PlayAnimation(idle, true);
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

		if (HUDWidget && TeamNum == 0)
		{
			HUDWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		if (current_cell_->GetGridPointer()->North(current_cell_) == tmp->GetID())
		{
			FRotator rot = FRotator(0.0f, -180.0f, 0.0f);
			mesh_->SetWorldRotation(rot);
		}
		else if (current_cell_->GetGridPointer()->South(current_cell_) == tmp->GetID())
		{
			FRotator rot = FRotator(0.0f, 0.0f, 0.0f);
			mesh_->SetWorldRotation(rot);
		}
		else if (current_cell_->GetGridPointer()->East(current_cell_) == tmp->GetID())
		{
			FRotator rot = FRotator(0.0f, -90.0f, 0.0f);
			mesh_->SetWorldRotation(rot);
		}
		else if (current_cell_->GetGridPointer()->West(current_cell_) == tmp->GetID())
		{
			FRotator rot = FRotator(0.0f, 90.0f, 0.0f);
			mesh_->SetWorldRotation(rot);
		}

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
					if (HUDWidget && TeamNum == 0)
					{
						HUDWidget->SetVisibility(ESlateVisibility::Visible);
					}
					mesh_->PlayAnimation(idle, true);
					bIsMoving = false;
				}
			}
		}
	}
}

void ACustomCharacter::Attack(ACell* cell_)
{
	ACell* character_cell = GetCell();
	ACustomCharacter* tmp_char = cell_->GetCharacterPointer();

	if (character_cell && !bHasAttackedThisTurn && !bHasHealedThisTurn && GetCharacterTeam() != tmp_char->GetCharacterTeam())
	{
		AGrid* grid_ = character_cell->GetGridPointer();
		if (grid_)
		{
			//float distance_to_cell = grid_->CalculateDistance(nullptr, nullptr);
			int distance_to_cell = grid_->CalculateManhattanDistance(character_cell, cell_);
			if (distance_to_cell <= 2)
			{
				if (tmp_char)
				{
					if (!tmp_char->bDied) {
						//if (tmp_char->GetCharacterTeam() != character_->GetCharacterTeam())
						//{
						if (character_cell->GetGridPointer()->North(character_cell) == cell_->GetID())
						{
							FRotator rot = FRotator(0.0f, -180.0f, 0.0f);
							mesh_->SetWorldRotation(rot);
							rot = FRotator(0.0f, 0.0f, 0.0f);
							tmp_char->mesh_->SetWorldRotation(rot);
						}
						else if (character_cell->GetGridPointer()->South(character_cell) == cell_->GetID())
						{
							FRotator rot = FRotator(0.0f, 0.0f, 0.0f);
							mesh_->SetWorldRotation(rot);
							rot = FRotator(0.0f, -180.0f, 0.0f);
							tmp_char->mesh_->SetWorldRotation(rot);
						}
						else if (character_cell->GetGridPointer()->East(character_cell) == cell_->GetID())
						{
							FRotator rot = FRotator(0.0f, -90.0f, 0.0f);
							mesh_->SetWorldRotation(rot);
							rot = FRotator(0.0f, 90.0f, 0.0f);
							tmp_char->mesh_->SetWorldRotation(rot);
						}
						else if (character_cell->GetGridPointer()->West(character_cell) == cell_->GetID())
						{
							FRotator rot = FRotator(0.0f, 90.0f, 0.0f);
							mesh_->SetWorldRotation(rot);
							rot = FRotator(0.0f, -90.0f, 0.0f);
							tmp_char->mesh_->SetWorldRotation(rot);
						}
						FVector Direction;
						FHitResult Hit;

						UGameplayStatics::ApplyPointDamage(tmp_char, 50.0f, Direction, Hit, PC, this, tmp_char->MeleeDamage);

						Unselected();
						mesh_->PlayAnimation(attack, false);
						GetWorldTimerManager().SetTimer(AttackTimer, this, &ACustomCharacter::FinishAttack, 0.7f, false);
					}
				}
			}
		}
	}
}


void ACustomCharacter::FinishAttack()
{
	mesh_->PlayAnimation(idle, true);

	if (bHasAttackedThisTurn)
	{
		HUDWidget->AttackButton->SetColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
		HUDWidget->DefenseButton->SetColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
		HUDWidget->HealButton->SetColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
	}

	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void ACustomCharacter::Defend()
{
	isDefending = true;
	mesh_->PlayAnimation(defense, true);

}

void ACustomCharacter::Heal(ACell* cell_)
{
	ACell* character_cell = GetCell();

	if (character_cell && !bHasAttackedThisTurn && !bHasHealed && !bHasHealedThisTurn)
	{
		AGrid* grid_ = character_cell->GetGridPointer();
		if (grid_)
		{
			int distance_to_cell = grid_->CalculateManhattanDistance(character_cell, cell_);
			if (distance_to_cell <= 2)
			{
				ACustomCharacter* tmp_char = cell_->GetCharacterPointer();
				if (tmp_char)
				{
					if (!tmp_char->bDied) {
						if (character_cell->GetGridPointer()->North(character_cell) == cell_->GetID())
						{
							FRotator rot = FRotator(0.0f, -180.0f, 0.0f);
							mesh_->SetWorldRotation(rot);
							rot = FRotator(0.0f, 0.0f, 0.0f);
							tmp_char->mesh_->SetWorldRotation(rot);
						}
						else if (character_cell->GetGridPointer()->South(character_cell) == cell_->GetID())
						{
							FRotator rot = FRotator(0.0f, 0.0f, 0.0f);
							mesh_->SetWorldRotation(rot);
							rot = FRotator(0.0f, -180.0f, 0.0f);
							tmp_char->mesh_->SetWorldRotation(rot);
						}
						else if (character_cell->GetGridPointer()->East(character_cell) == cell_->GetID())
						{
							FRotator rot = FRotator(0.0f, -90.0f, 0.0f);
							mesh_->SetWorldRotation(rot);
							rot = FRotator(0.0f, 90.0f, 0.0f);
							tmp_char->mesh_->SetWorldRotation(rot);
						}
						else if (character_cell->GetGridPointer()->West(character_cell) == cell_->GetID())
						{
							FRotator rot = FRotator(0.0f, 90.0f, 0.0f);
							mesh_->SetWorldRotation(rot);
							rot = FRotator(0.0f, -90.0f, 0.0f);
							tmp_char->mesh_->SetWorldRotation(rot);
						}
						FVector Direction;
						FHitResult Hit;

						UGameplayStatics::ApplyPointDamage(tmp_char, -50.0f, Direction, Hit, PC, this, tmp_char->MeleeDamage);

						Unselected();
						mesh_->PlayAnimation(heal, false);
						GetWorldTimerManager().SetTimer(HealTimer, this, &ACustomCharacter::FinishHeal, 2.5f, false);
					}
				}
			}
		}
	}
}


void ACustomCharacter::FinishHeal()
{
	mesh_->PlayAnimation(idle, true);

	if (bHasHealedThisTurn)
	{
		HUDWidget->AttackButton->SetColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
		HUDWidget->DefenseButton->SetColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
		HUDWidget->HealButton->SetColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
	}

	GetWorldTimerManager().ClearTimer(HealTimer);
}

void ACustomCharacter::EndTurn()
{
	bTurnAvailable = false;

	Unselected();

	HUDWidget->selected_action_ = kSelectedAction_None;
	HUDWidget->SetVisibility(ESlateVisibility::Hidden);

	//Sets the view target to the camera pawn.
	GetWorldTimerManager().SetTimer(handle_, this, &ACustomCharacter::ReturnToMainCamera, 0.3f, false);
}

void ACustomCharacter::Die() {
	bDied = true;
	bTurnAvailable = false;

	current_cell_->SetCharacterPointer(nullptr);

	//Remove Pointer fromm team.
	ACustomGameMode* GM = Cast<ACustomGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GM) {
		for (int t_idx = 0; t_idx < GM->GameTeams.Num(); t_idx++) {
			if (GM->GameTeams[t_idx].TeamId == GetCharacterTeam()) {
				GM->GameTeams[t_idx].TeamMembers.Remove(this);
				if (GM->GameTeams[t_idx].TeamAIController != nullptr) {
					GM->GameTeams[t_idx].TeamAIController->AITeamCharacters.Remove(this);
				}
			}
		}
	}
}

void ACustomCharacter::StartTurn()
{
	bTurnAvailable = true;
	isDefending = false;
	bHasAttackedThisTurn = false;
	bHasHealedThisTurn = false;
	cells_moved_this_turn_ = 0;

  if (HUDWidget)
	{
		HUDWidget->AttackButton->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		HUDWidget->DefenseButton->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		if (!bHasHealed)
		{
			HUDWidget->HealButton->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else
		{
			HUDWidget->HealButton->SetColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
		}
	}
}

void ACustomCharacter::StartVictoryAnimation()
{
	mesh_->PlayAnimation(victory, true);
}


void ACustomCharacter::ReturnToMainCamera()
{
	GetWorldTimerManager().ClearTimer(handle_);
	state_ = CharacterState::kCharacterState_Idle;

	PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
	if (PC) {
		PC->SetViewTargetWithBlend(camera_pawn_, 1.0f);
		GetWorldTimerManager().SetTimer(handle_, this, &ACustomCharacter::EndCharacterTurn, 1.0f, false);
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
		int north, east, south, west = 0;

		//North
		north = grid->North(current_cell_);

		if (grid->IsValidID(north))
		{
			if (!grid->isWall(grid->Cells[north]))
			{
				cells.Add(grid->Cells[north]);
			}
		}

		//East
		east = grid->East(current_cell_);

		if (grid->IsValidID(east))
		{
			if (!grid->isWall(grid->Cells[east]))
			{
				cells.Add(grid->Cells[east]);
			}
		}
		
		//South
		south = grid->South(current_cell_);

		if (grid->IsValidID(south))
		{
			if (!grid->isWall(grid->Cells[south]))
			{
				cells.Add(grid->Cells[south]);
			}
		}

		//West
		west = grid->West(current_cell_);

		if (grid->IsValidID(west))
		{
			if (!grid->isWall(grid->Cells[west]))
			{
				cells.Add(grid->Cells[west]);
			}
		}
	}

	return cells;
}

void ACustomCharacter::Selected()
{
	//Makes the HUD visible and sets the character name
	if (HUDWidget)
	{
		HUDWidget->current_character_ = this;
		HUDWidget->SetCharacterName(name_);
		HUDWidget->SetVisibility(ESlateVisibility::Visible);
	}
	
	mesh_->PlayAnimation(idle, true);


	//Highlights the action cells.
	//current_cell_->GetGridPointer()->HighlightCells(GetSelectableCells());
	//current_cell_->HighlightCell(CellMaterial::kCellMaterial_CurrentCell);
}

void ACustomCharacter::Unselected()
{
	//Unhighlights the action cells.
	AGrid* grid = current_cell_->GetGridPointer();

	if (grid)
	{
		grid->UnhighlightCells(GetMovableCells());
		grid->UnhighlightCells(GetAttackCells());
	}
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

void ACustomCharacter::OnHealthChanged(UHealthComponent * OwningHealthComp, float CurrentHealth, float HealthDelta, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (CurrentHealth <= 0.0f && !bDied)
	{
		Die();
		mesh_->PlayAnimation(death, false);

		//DetachFromControllerPendingDestroy();

		SetLifeSpan(5.0f);
	}
}


void ACustomCharacter::EndCharacterTurn() {
	bTurnAvailable = false;
}
