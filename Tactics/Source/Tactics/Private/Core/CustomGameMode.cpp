#include "Core/CustomGameMode.h"

#include "Grid/Cell.h"
#include "Grid/Grid.h"
#include "Character/CustomCharacter.h"
#include "Core/SoundManager.h"

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

void ACustomGameMode::CheckVictoryCondition() 
{
	//DEFEAT CONDITION (assuming player is team 0)
	bool defeat = true;

	for (int i = 0; i < GameTeams[0].TeamMembers.Num(); ++i)
	{
		if (!GameTeams[0].TeamMembers[i]->bDied) {
			defeat = false;
			break;
		}
	}

	if (defeat)
	{
		//DEFEAT
		UE_LOG(LogTemp, Warning, TEXT("GAME OVER"));

		for (int i = 1; i < GameTeams.Num(); ++i)
		{
			for (int j = 0; j < GameTeams[i].TeamMembers.Num(); ++j)
			{
				if (!GameTeams[i].TeamMembers[j]->bDied) {
					GameTeams[i].TeamMembers[j]->mesh_->PlayAnimation(GameTeams[0].TeamMembers[i]->victory, false);
				}
			}
		}

		return;
	}

	//VICTORY CONDITION (assuming player is team 0)
	bool victory = true;

	for (int i = 1; i < GameTeams.Num() && victory; ++i)
	{
		for (int j = 0; j < GameTeams[i].TeamMembers.Num(); ++j)
		{
			if (!GameTeams[i].TeamMembers[j]->bDied) {
				victory = false;
				break;
			}
		}
	}

	if (victory)
	{
		//VICTORY
		UE_LOG(LogTemp, Warning, TEXT("VICTORY"));

		for (int i = 0; i < GameTeams[0].TeamMembers.Num(); ++i)
		{
			if (!GameTeams[0].TeamMembers[i]->bDied) {
				GameTeams[0].TeamMembers[i]->mesh_->PlayAnimation(GameTeams[0].TeamMembers[i]->victory, false);
			}
		}

		return;
	}
}