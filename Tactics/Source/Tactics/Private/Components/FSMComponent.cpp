#include "Components/FSMComponent.h"
#include "Character/CustomCharacter.h"
#include "Components/HealthComponent.h"
#include "Grid/Cell.h"

UFSMComponent::UFSMComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UFSMComponent::BeginPlay()
{
	Super::BeginPlay();

	state_ = CharacterStates::kCharacterState_IDLE;
	sub_state_ = CharacterStates::kCharacterState_IDLE;
	owner_ = Cast<ACustomCharacter>(GetOwner());
	target_character_ = nullptr;
	target_cell_ = nullptr;

	bUsedAction_ = false;
	bUsedHeal_ = false;
	bMoved_ = false;
	bTargetReached_ = false;
}

void UFSMComponent::BeginTurn() {
	bUsedAction_ = false;
	bMoved_ = false;
}

void UFSMComponent::UpdateCharacterState() {
	if ((owner_->HealthComp)->GetCurrentHealthPercentage() < 0.5) {
		state_ = CharacterStates::kCharacterState_DEFFENSIVE;
		sub_state_ = CharacterStates::kCharacterState_CHASE;
	}
	else {
		state_ = CharacterStates::kCharacterState_OFFENSIVE;
		sub_state_ = CharacterStates::kCharacterState_CHASE;
	}
}

void UFSMComponent::UpdateCharacterSubState() {
	if (state_ == CharacterStates::kCharacterState_OFFENSIVE) {
		if (target_character_ != nullptr && bTargetReached_) {
			sub_state_ = CharacterStates::kCharacterState_ATTACK;
		}
		else if (target_character_ != nullptr && !bTargetReached_) {
			sub_state_ = CharacterStates::kCharacterState_CHASE;
		}
		else {
			sub_state_ = CharacterStates::kCharacterState_IDLE;
		}
	} else if (state_ == CharacterStates::kCharacterState_DEFFENSIVE) {
		if (!bUsedHeal_) {
			sub_state_ = CharacterStates::kCharacterState_HEAL;
		}
		else {
			if (bMoved_ && target_character_ != nullptr) {
				sub_state_ = CharacterStates::kCharacterState_DEFEND;
			}
			else if (bMoved_ && target_character_ != nullptr)  {
				sub_state_ = CharacterStates::kCharacterState_RETREAT;
			} 
			else {
				sub_state_ = CharacterStates::kCharacterState_IDLE;
			}
		}
	} else { 
		sub_state_ = CharacterStates::kCharacterState_IDLE;
	}
}


void UFSMComponent::CharacterAction() {
	switch (sub_state_)
	{
	case kCharacterState_IDLE:
		owner_->EndCharacterTurn();
		break;
	case kCharacterState_CHASE:
		Chase();
		owner_->EndCharacterTurn();
		break;
	case kCharacterState_RETREAT:
		break;
	case kCharacterState_ATTACK:
		break;
	case kCharacterState_DEFEND:
		break;
	default:
		break;
	}
}

void UFSMComponent::Chase() {

	//Comprobar los vecinos del target para ver si son cells disponibles, y si estan en rango de movimiento.
	//Asi los enemigos podrian roedear al target en caso de que esta opcion estuviese disponible.

	TArray<ACell*> movable_cells = owner_->GetMovableCells();;
	TArray<ACell*> path;

	if (game_grid_ && owner_ && target_character_ && target_cell_) {
		path = game_grid_->FindPath(owner_->GetCell(), target_cell_);
		movable_cells = owner_->GetMovableCells();

		ACell* target_cell = nullptr;

		for (int path_cell_idx = path.Num()-1; path_cell_idx >= 0; --path_cell_idx) {
			for (int m_cells = 0; m_cells < movable_cells.Num(); ++m_cells) {
				if (path[path_cell_idx] == movable_cells[m_cells]) {
					target_cell = path[path_cell_idx];
				}
			}
		}

		if(target_cell != nullptr) game_grid_->MoveCharacterToCell(owner_, target_cell);
		owner_->mesh_->PlayAnimation(owner_->walk, true);
	}	
}


void UFSMComponent::SetTargetCharacter(ACustomCharacter* new_target) {
	target_character_ = new_target;
}

void UFSMComponent::SetTargetCell(ACell* new_target) {
	target_cell_ = new_target;
}

void UFSMComponent::SetGrid(AGrid* new_grid) {
	game_grid_ = new_grid;
}

