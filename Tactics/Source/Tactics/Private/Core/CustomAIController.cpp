#include "Core/CustomAIController.h"

#include "CameraPawn/CameraPawn.h"
#include "Components/FSMComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void ACustomAIController::BeginTurn() {
	bIsMyTurn = true;

	for (int c_idx = 0; c_idx < AITeamCharacters.Num(); c_idx++) {
		AITeamCharacters[c_idx]->FiniteStateMachineComponent->BeginTurn();
		AITeamCharacters[c_idx]->FiniteStateMachineComponent->SetGrid(Grid);
	}
	SetNextCharacterForAction();
}

void ACustomAIController::SetCharacterRandomMovement() {

	GetWorldTimerManager().ClearTimer(TimerHandle);

	if (SelectedCharacter != nullptr) {
		SelectedCharacter->FiniteStateMachineComponent->CharacterAction();
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ACustomAIController::SetNextCharacterForAction, 2.0f, false);
	}
}


void ACustomAIController::SetNextCharacterForAction() {

	GetWorldTimerManager().ClearTimer(TimerHandle);

	SelectedCharacter = nullptr;

	for (int c_idx = 0; c_idx < AITeamCharacters.Num(); c_idx++) {
		if (AITeamCharacters[c_idx]->bTurnAvailable && SelectedCharacter == nullptr) {
			SelectedCharacter = AITeamCharacters[c_idx];
		}
	}
		
	if (SelectedCharacter != nullptr) {
		SetCharacterTarget();
		SelectedCharacter->FiniteStateMachineComponent->UpdateCharacterState();
	}
	
	APlayerController* player_controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (player_controller) {
		if (SelectedCharacter != nullptr) {
			player_controller->SetViewTargetWithBlend(SelectedCharacter, 1.0f);
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ACustomAIController::SetCharacterRandomMovement, 1.0f, false);
		}
		else {
			player_controller->SetViewTargetWithBlend(CameraPawn, 1.0f);
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ACustomAIController::EndTurn, 1.0f, false);
		}
	}
}

void ACustomAIController::EndTurn() {
	bIsMyTurn = false;
}

bool ACustomAIController::IsMyTurn() {
	return bIsMyTurn;
}

void ACustomAIController::SetCharacterTarget() {

	int distance = Grid->Cells.Num();
	TArray<ACell*> neighbours;
	TArray<ACell*> path;
	ACell* target_cell = nullptr;
	ACustomCharacter* target_character = nullptr;
	bool target_reached = false;

	if (SelectedCharacter != nullptr) {
		//CHeck for Character Target Next To Our Character. (set that into target_reached in FSM).
		neighbours = SelectedCharacter->GetCell()->GetNeighbours();
		for (int n_idx = 0; n_idx < neighbours.Num() && target_reached == false ; ++n_idx) {
			for (int c_idx = 0; c_idx < OtherTeamsCharacters.Num() && target_reached == false; ++c_idx) {
				if (neighbours[n_idx]->GetCharacterPointer() == OtherTeamsCharacters[c_idx]) {
					target_reached = true;
					target_cell = neighbours[n_idx];
					target_character = OtherTeamsCharacters[c_idx];
				}
			}
		}

		//Look for the closest target available. 
		if (OtherTeamsCharacters.Num() > 0 && !target_reached) {
			for (int c_idx = 0; c_idx < OtherTeamsCharacters.Num(); ++c_idx) {
				if (OtherTeamsCharacters[c_idx] != nullptr) {
					neighbours = OtherTeamsCharacters[c_idx]->GetCell()->GetNeighbours();
					for (int n_idx = 0; n_idx < neighbours.Num(); ++n_idx) {
						path = Grid->FindPath(SelectedCharacter->GetCell(), neighbours[n_idx]);
						if (path.Num() != 0) {
							if (path.Num() < distance) {
								distance = path.Num();
								target_cell = neighbours[n_idx];
								target_character = OtherTeamsCharacters[c_idx];
							}
						}
					}
				}
			}
		}

		if (target_cell && target_character) {
			(SelectedCharacter->FiniteStateMachineComponent)->SetTargetCell(target_cell);
			(SelectedCharacter->FiniteStateMachineComponent)->SetTargetCharacter(target_character);
			(SelectedCharacter->FiniteStateMachineComponent)->SetTargetReached(target_reached);
		}
	}


}
