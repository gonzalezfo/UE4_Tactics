#include "CustomGameMode.h"

#include "Grid.h"

#include "Kismet/GameplayStatics.h"

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

void ACustomGameMode::SetGameTeamsFromGridSpawns(AGrid* grid) {
	if (grid != nullptr) {
		for (int spawns = 0; spawns < grid->Spawns.Num(); spawns++) {
			
		}
	}
}