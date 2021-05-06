#include "Core/CustomGameMode.h"

#include "Grid/Cell.h"
#include "Grid/Grid.h"
#include "Character/CustomCharacter.h"
#include "Core/SoundManager.h"
#include "Widgets/VictoryOrDefeatWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Engine/Public/UnrealEngine.h"

#include "GameFramework/Actor.h"


ACustomGameMode::ACustomGameMode() {
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACustomGameMode::BeginPlay()
{
	Super::BeginPlay();

	//Find the Grid for the GameMode.
	GameGrid = Cast<AGrid>(UGameplayStatics::GetActorOfClass(GetWorld(), AGrid::StaticClass()));

	GridCamera = Cast<ACameraPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), ACameraPawn::StaticClass()));

	PlayerController = Cast<ACustomPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (GameGrid != nullptr) {
		GameGrid->Init();
		SetGameTeamsFromGridSpawns(GameGrid);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;	

	SoundManager = GetWorld()->SpawnActor<ASoundManager>(SoundManagerClass,
		FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnParams);

	LevelFinished = false;
}

void ACustomGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Check Victory Condition
	CheckVictoryCondition();

	//NextTurn if Game Continues.
	if (!LevelFinished) {
		bool next_turn = false;
		next_turn = CheckForNextTurn();
		if (next_turn) NextTurn();

		FColor text_color = FColor::Purple;

		// Team Turn DISPLAY // TODO: MOVE TO WIDGET OR SOMETHING. DELETE THIS LATER.
		switch (GameTeams[CurrentTeamTurn].TeamId) {
		case 0: text_color = FColor::Green; break;
		case 1: text_color = FColor::Red; break;
		case 2: text_color = FColor::Blue; break;
		case 3: text_color = FColor::Yellow; break;
		default: text_color = FColor::Purple; break;
		}
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, text_color, FString::Printf(TEXT("Team Turn: %d"), CurrentTeamTurn));

	}
}

void ACustomGameMode::SetGameTeamsFromGridSpawns(AGrid* grid) {
	//Save the different Teams that our Grid Sapwns have already generate.
	//First Loop just checks the number of different teams that are accross all our spawns.
	if (grid != nullptr) {
		for (int spawn = 0; spawn < grid->Spawns.Num(); spawn++) {
			FTeam new_team;
			new_team.TeamId = grid->Spawns[spawn].SpawnTeam;
			bool valid_team = true;
			for (int teams = 0; teams < GameTeams.Num(); teams++) {
				if (GameTeams[teams].TeamId == new_team.TeamId) valid_team = false;
			}
			if (valid_team) GameTeams.Push(new_team);
		}

		for (int s_idx = 0; s_idx < grid->Spawns.Num(); s_idx++) {
			int team_index = 0;
			for (int t_idx = 0; t_idx < GameTeams.Num(); t_idx++) {
				if (GameTeams[t_idx].TeamId == grid->Spawns[s_idx].SpawnTeam) {
					team_index = t_idx;
				} 
			}
			for (int c_idx = 0; c_idx < grid->Spawns[s_idx].SpawnCells.Num(); c_idx++) {
				ACustomCharacter* character = (grid->Spawns[s_idx].SpawnCells[c_idx])->GetCharacterPointer();
				GameTeams[team_index].TeamMembers.Push(character);
			}
		}
	}

	//Spawn the AIControllers and set their values depending on their teams.
	for (int t_idx = 0; t_idx < GameTeams.Num(); t_idx++) {
		if (GameTeams[t_idx].TeamId != ESpawnTeam::kSpawnTeam_Player) {
			ACustomAIController* ai_controller = Cast<ACustomAIController>(GetWorld()->SpawnActor<AActor>(ACustomAIController::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator));
			ai_controller->AITeamCharacters = GameTeams[t_idx].TeamMembers;
			ai_controller->CameraPawn = GridCamera;
			ai_controller->Grid = GameGrid;

			if (ai_controller != nullptr) {
				GameAIPlayers.Push(ai_controller);
				GameTeams[t_idx].TeamAIController = ai_controller;
			}
		} else {
			GameTeams[t_idx].TeamAIController = nullptr;
		}
	}

	//Let AIControllers know about the other teams characters information.
	for (int t_idx = 0; t_idx < GameTeams.Num(); t_idx++) {
		if (GameTeams[t_idx].TeamAIController != nullptr) {
			for (int t_idx_2 = 0; t_idx_2 < GameTeams.Num(); t_idx_2++) {
				if (t_idx != t_idx_2) {
					for (int c_idx = 0; c_idx < GameTeams[t_idx_2].TeamMembers.Num(); c_idx++) {
						GameTeams[t_idx].TeamAIController->OtherTeamsCharacters.Push(GameTeams[t_idx_2].TeamMembers[c_idx]);
					}
				}
			}
		}
	}

	//Set things for turns
	CurrentTeamTurn = 0;
	SetTurn(0);
}

//Sets the movement aviability of all the characters of the Teams.
void ACustomGameMode::SetTurn(int value) {
	if (value >= GameTeams.Num()) return;

	for (auto& characters : GameTeams[value].TeamMembers) {
		characters->StartTurn();
	}

	if (GameTeams[value].TeamId != ESpawnTeam::kSpawnTeam_Player) {
		if (GameTeams[value].TeamAIController != nullptr) {
			GameTeams[value].TeamAIController->BeginTurn();
			GameTeams[value].bFinishedTurn = false;
		}
	}
	else {
		PlayerController->BeginTurn();
		GameTeams[value].bFinishedTurn = false;
	}
}

void ACustomGameMode::NextTurn() {
	CurrentTeamTurn++;
	if (CurrentTeamTurn >= GameTeams.Num()) CurrentTeamTurn = 0; 
	SetTurn(CurrentTeamTurn);
}

bool ACustomGameMode::CheckForNextTurn() {
	if (GameTeams[CurrentTeamTurn].TeamId != ESpawnTeam::kSpawnTeam_Player) {
		GameTeams[CurrentTeamTurn].bFinishedTurn = !(GameTeams[CurrentTeamTurn].TeamAIController)->IsMyTurn();
		return GameTeams[CurrentTeamTurn].bFinishedTurn;
	}
	else {
		if (PlayerController->IsMyTurn() && GameTeams[CurrentTeamTurn].bFinishedTurn == false) {
			bool finished_players_turn = true;
			for (int c_idx = 0; c_idx < GameTeams[CurrentTeamTurn].TeamMembers.Num(); c_idx++) {
				if ((GameTeams[CurrentTeamTurn].TeamMembers[c_idx])->bTurnAvailable == true) {
					finished_players_turn = false;
				}
			}
			if (finished_players_turn) {
				GetWorldTimerManager().SetTimer(TimerHandle, PlayerController, &ACustomPlayerController::EndTurn, 2.0f, false);
				GameTeams[CurrentTeamTurn].bFinishedTurn = true;
			}
		}
		return !(PlayerController->IsMyTurn());
	}
	return false;
}

void ACustomGameMode::VictoryCelebration()
{
	int player_idx = 0;

	// Set player_idx to look for the player team inside our GameTeams Array.
	for (int t_idx = 0; t_idx < GameTeams.Num(); ++t_idx) {
		if (GameTeams[t_idx].TeamId == ESpawnTeam::kSpawnTeam_Player) {
			player_idx = t_idx;
		}
	}
	for (int i = 0; i < GameTeams[player_idx].TeamMembers.Num(); ++i)
	{
		GameTeams[player_idx].TeamMembers[i]->mesh_->PlayAnimation(GameTeams[player_idx].TeamMembers[i]->victory, true);
	}

	GetWorldTimerManager().ClearTimer(VictoryTimer);

}

void ACustomGameMode::CheckVictoryCondition() 
{
	int player_idx = 0;

	// Set player_idx to look for the player team inside our GameTeams Array.
	for (int t_idx = 0; t_idx < GameTeams.Num(); ++t_idx) {
		if (GameTeams[t_idx].TeamId == ESpawnTeam::kSpawnTeam_Player) {
			player_idx = t_idx;
		}
	}

	//Check Defeated Teams.
	for (int t_idx = 0; t_idx < GameTeams.Num(); ++t_idx) {
		bool team_defeated = true;
		if (GameTeams[t_idx].TeamMembers.Num() != 0) {
			team_defeated = false;
		}

		GameTeams[t_idx].Defeated = team_defeated;
	}

	//Check Victory of the player with the player team idx.
	bool player_defeat = true;

	if (!GameTeams[player_idx].Defeated) {
		player_defeat = false;
	}

	if (player_defeat)
	{
		//DEFEAT
		UE_LOG(LogTemp, Warning, TEXT("GAME OVER"));
    
		if (GridCamera->VictoryOrDefeatWidget && !GridCamera->VictoryOrDefeatWidget->IsInViewport())
		{
			for (int i = 0; i < GameTeams.Num(); ++i) {
				if (i != player_idx) {
					for (int j = 0; j < GameTeams[i].TeamMembers.Num(); ++j)
					{
						if (!GameTeams[i].TeamMembers[j]->bDied) {
							GameTeams[i].TeamMembers[j]->mesh_->PlayAnimation(GameTeams[i].TeamMembers[j]->victory, true);
						}
					}
				}
			}
			GridCamera->VictoryOrDefeatWidget->AddToViewport();
			GridCamera->VictoryOrDefeatWidget->InitWidget(false, 0.0f, (float)number_of_players);
		}


		LevelFinished = true;

		return;
	}

	bool player_victory = true;

	for (int t_idx = 0; t_idx < GameTeams.Num() && player_victory; ++t_idx) {
		if (t_idx != player_idx) {
			if (!GameTeams[t_idx].Defeated) {
				player_victory = false;
			}
		}
	}

	if (player_victory)
	{
		//VICTORY
		UE_LOG(LogTemp, Warning, TEXT("VICTORY"));

		if (GridCamera->VictoryOrDefeatWidget && !GridCamera->VictoryOrDefeatWidget->IsInViewport())
		{
			int players_alive = 0;
			for (int i = 0; i < GameTeams[player_idx].TeamMembers.Num(); ++i)
			{
				if (!GameTeams[player_idx].TeamMembers[i]->bDied) {
					GameTeams[player_idx].TeamMembers[i]->EndTurn();
					players_alive++;
				}
			}
			GridCamera->VictoryOrDefeatWidget->AddToViewport();
			GridCamera->VictoryOrDefeatWidget->InitWidget(true, (float)players_alive, (float)number_of_players);

			GetWorldTimerManager().SetTimer(VictoryTimer, this, &ACustomGameMode::VictoryCelebration, 2.0f, false);
		}


		LevelFinished = true;

		return;
	}
}