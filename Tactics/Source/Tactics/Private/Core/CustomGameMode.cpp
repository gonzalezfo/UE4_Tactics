#include "Core/CustomGameMode.h"

#include "Grid/Cell.h"
#include "Grid/Grid.h"
#include "Character/CustomCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Engine/Public/UnrealEngine.h"

ACustomGameMode::ACustomGameMode() {
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void ACustomGameMode::BeginPlay()
{
	Super::BeginPlay();

	//Find the Grid for the GameMode.
	GameGrid = (AGrid*) UGameplayStatics::GetActorOfClass(GetWorld(), AGrid::StaticClass());

	if (GameGrid != nullptr) {
		GameGrid->Init();
		SetGameTeamsFromGridSpawns(GameGrid);
	}
}

void ACustomGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//NextTurn.
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
	//
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
	//Set things for turns
	CurrentTeamTurn = 0;
	SetTurn(0);
}

//Sets the movement aviability of all the characters of the Teams.
void ACustomGameMode::SetTurn(int value) {
	if (value >= GameTeams.Num()) return;
	/*for (int t_idx = 0; t_idx < GameTeams.Num(); t_idx++) {
		bool movement = (t_idx == CurrentTeamTurn);

		for (int c_idx = 0; c_idx < GameTeams[t_idx].TeamMembers.Num(); c_idx++) {
			(GameTeams[t_idx].TeamMembers[c_idx])->TurnAvailable = movement;
		}
	}*/
	for (auto& characters : GameTeams[value].TeamMembers) {
		characters->StartTurn();
	}
	
}

void ACustomGameMode::NextTurn() {
	CurrentTeamTurn++;
	if (CurrentTeamTurn >= GameTeams.Num()) CurrentTeamTurn = 0; 
	SetTurn(CurrentTeamTurn);
}

bool ACustomGameMode::CheckForNextTurn() {
	for (int c_idx = 0; c_idx < GameTeams[CurrentTeamTurn].TeamMembers.Num(); c_idx++) {
		if ((GameTeams[CurrentTeamTurn].TeamMembers[c_idx])->TurnAvailable == true) return false;
	}
	return true;
}