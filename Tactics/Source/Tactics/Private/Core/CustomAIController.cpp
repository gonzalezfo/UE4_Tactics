#include "Core/CustomAIController.h"

#include "CameraPawn/CameraPawn.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void ACustomAIController::BeginTurn() {
	SetNextCharacterForMovement();
	bIsMyTurn = true;
}

void ACustomAIController::SetCharacterRandomMovement() {

	GetWorldTimerManager().ClearTimer(TimerHandle);

	if (SelectedCharacter != nullptr) {
		TArray<ACell*> selectable_cells = SelectedCharacter->GetMovableCells();
		if (selectable_cells.Num() > 0 && Grid != nullptr) {
			int value = FMath::RandRange(0, selectable_cells.Num() - 1);
			Grid->MoveCharacterToCell(SelectedCharacter, selectable_cells[value]);
			SelectedCharacter->TurnAvailable = false;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ACustomAIController::SetNextCharacterForMovement, 2.0f, false);
		}
	}
}


void ACustomAIController::SetNextCharacterForMovement() {

	GetWorldTimerManager().ClearTimer(TimerHandle);

	SelectedCharacter = nullptr;

	for (int c_idx = 0; c_idx < AITeamCharacters.Num(); c_idx++) {
		if (AITeamCharacters[c_idx]->TurnAvailable && SelectedCharacter == nullptr) {
			SelectedCharacter = AITeamCharacters[c_idx];
		}
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
