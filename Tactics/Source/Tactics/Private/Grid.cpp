// Fill out your copyright notice in the Description page of Project Settings.

#include "Grid.h"
#include "Cell.h"
#include "CustomCharacter.h"
#include "DrawDebugHelpers.h"

// Sets default values
AGrid::AGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GridSize = FIntPoint(5, 5);

	SquareWidth = 200.0f;

	RoomLength = SquareWidth * GridSize.Y;
	RoomWidth = SquareWidth* GridSize.X;
}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();	

	CreateGrid();
}

// Called every frame
void AGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector AGrid::GetCellSize()
{
	FVector tmp(SquareWidth, SquareWidth, 1.0f);

	return tmp;
}

bool AGrid::IsValidID(ACell* cell)
{
	if (cell->GetID() >= 0 && cell->GetID() < (GridSize.X * GridSize.Y))
	{
		return true;
	}

	return false;
}

bool AGrid::isWall(ACell* cell)
{
	if (!IsValidID(cell))
	{
		return true;
	}

	if (Cells[cell->GetID()]->GetType() == kCellType_Wall)
	{
		return true;
	}
	return false;
}

bool AGrid::IsValidRowCol(int row, int col)
{
	return (row >= 0 && row < GridSize.Y && col >= 0 && col < GridSize.X);
}

int AGrid::North(ACell* origin)
{
	if (!IsValidID(origin))
	{
		return -1;
	}

	int const index = origin->GetID() - GridSize.X;

	if (index < 0) {
		return -1;
	}

	return index;
}

int AGrid::South(ACell* origin)
{
	if (!IsValidID(origin))
	{
		return -1;
	}

	int const index = origin->GetID() + GridSize.X;

	if (index > GridSize.X * GridSize.Y)
	{
		return -1;
	}

	return index;
}

int AGrid::East(ACell* origin)
{
	if (!IsValidID(origin))
	{
		return -1;
	}

	int row, col = -1;
	IndexToRowCol(&row, &col, origin);

	if (col == GridSize.X - 1)
	{
		return -1;
	}

	return origin->GetID() + 1;
}

int AGrid::West(ACell* origin)
{
	if (!IsValidID(origin))
	{
		return -1;
	}

	int row, col = -1;
	IndexToRowCol(&row, &col, origin);

	if (col == 0)
	{
		return -1;
	}

	return origin->GetID() - 1;
}

void AGrid::MoveCharacterToCell(ACustomCharacter* character, ACell* new_cell)
{
	if (character && new_cell)
	{
		ACell* tmp = character->GetCell();

		if (tmp)
		{
			tmp->SetCharacterPointer(nullptr);
		}

		character->SetCell(new_cell);
		new_cell->SetCharacterPointer(character);
	}
}


void AGrid::IndexToRowCol(int* row, int* col, ACell* origin)
{
	if (!IsValidID(origin))
	{
		return;
	}

	*row = origin->GetID() / GridSize.X;
	*col = origin->GetID() % GridSize.Y;
}


AActor* AGrid::SpawnItem(UClass* ItemToSpawn, FVector& Position)
{
	FRotator rotation (0.0f);

	return GetWorld()->SpawnActor<AActor>(ItemToSpawn, Position, rotation);
}

static TArray<CellType> GenerateGridWalkers(FIntPoint gridSize, int walkersNum, int iterations) {
	
	//Set CellType Array
	TArray<CellType> cell_types;
	cell_types.Init(kCellType_Wall, gridSize.X * gridSize.Y);

	//Set walkers. (Maybe turn this into a struct)
	TArray<FIntPoint> grid_walkers_positions;
	grid_walkers_positions.Init(FIntPoint(0, 0), walkersNum);

	TArray<int> grid_walkers_directions;
	grid_walkers_directions.Init(0, walkersNum);

	// Spawn Walkers in our grid. Init their values.
	for (int i = 0; i < walkersNum; i++) {
		grid_walkers_positions[i] = FIntPoint(FMath::RandRange(0, gridSize.X - 1), FMath::RandRange(1, gridSize.Y - 1));
		grid_walkers_directions[i] = FMath::RandRange(0, 3);
	}
	
	//Path Generation Loop.
	int iterator = 0;
	while (iterator < iterations) {
		//Check walkers valid direction & move them | change direction.
		for (int i = 0; i < grid_walkers_positions.Num(); i++) {
			bool is_valid_dir = false;
			FIntPoint new_position = grid_walkers_positions[i];
			switch (grid_walkers_directions[i])
			{
			case 0:
				new_position.Y += 1;
				break;
			case 1:
				new_position.X += 1;
				break;
			case 2:
				new_position.Y -= 1;
				break;
			case 3:
				new_position.X -= 1;
				break;
			}
			is_valid_dir = (new_position.Y >= 0 && new_position.Y < gridSize.Y &&
				new_position.X >= 0 && new_position.X < gridSize.X);
			if (is_valid_dir) {
				grid_walkers_positions[i] = new_position;
			}
			if (cell_types[grid_walkers_positions[i].Y * gridSize.X + grid_walkers_positions[i].X] != kCellType_Spawn) {
				cell_types[grid_walkers_positions[i].Y * gridSize.X + grid_walkers_positions[i].X] = kCellType_Normal;
			}
			grid_walkers_directions[i] = FMath::RandRange(0, 3);

		}
		iterator++;
	}
	//
	return cell_types;
}

static TArray<CellType> GenerateGridPerlin(FIntPoint size, float obstaclePercentaje, FVector2D obstacleDiffusion) {
	TArray<float> cell_values_perlin;
	cell_values_perlin.Init(0.0f, size.X * size.Y);

	TArray<CellType> cell_types;
	cell_types.Init(kCellType_Void, size.X * size.Y);

	float pseudo_seed = FMath::FRandRange(-1.0f, 1.0f);

	for (int i = 0; i < size.Y; ++i) {
		for (int j = 0; j < size.X; ++j) {
			int index = i * size.X + j;
			FVector location = { i * obstacleDiffusion.X , j * obstacleDiffusion.Y ,  pseudo_seed };
			float value = FMath::PerlinNoise3D(location) + 1.0f;
			value = value / 2.0f; //renormalize between 0 and 1

			cell_values_perlin[index] = value;

		}
	}
	TArray<float> tmp_values(cell_values_perlin);
	tmp_values.Sort();
	int percentaje_position;
	if (FMath::IsNearlyZero(obstaclePercentaje)) {
		percentaje_position = 0;
	}
	else {
		float ceiling = ceilf(obstaclePercentaje * tmp_values.Num());
		percentaje_position = int(ceiling) - 1;
	}
	float updatedPercentaje = tmp_values[percentaje_position];
	UE_LOG(LogTemp, Warning, TEXT("%f"), updatedPercentaje);

	//float updatedPercentaje = 0.0f;
	for (int i = 0; i < size.Y; ++i) {
		for (int j = 0; j < size.X; ++j) {
			int index = i * size.X + j;
			if (cell_values_perlin[index] < updatedPercentaje) {
				cell_types[index] = kCellType_Wall;
			}
			else {
				cell_types[index] = kCellType_Normal;
			}
		}
	}


	return cell_types;
}

static void GenerateGridSpawns(TArray<CellType> &grid, FIntPoint gridSize, int numSpawns, int spawnSize, int spawnMinDistance) {
	//SpawnSize Check
	if (spawnSize <= 0 || spawnSize > gridSize.X || spawnSize > gridSize.Y) spawnSize = 1;

	//SpawnMinDistance Check.
	if (spawnMinDistance <= 0 || spawnMinDistance > gridSize.X || spawnMinDistance > gridSize.Y) spawnMinDistance = (gridSize.X + gridSize.Y) / 2;

	//Set walkers. (Maybe turn this into a struct)
	TArray<FIntPoint> spawn_positions;
	spawn_positions.Init(FIntPoint(0, 0), numSpawns);
	TArray<int> spawn_directions;
	spawn_directions.Init(0, numSpawns);

	bool valid_spawn_positions = true;
	bool valid_spawn_distances = true;

	do {
		valid_spawn_distances = true;
		// Spawn Spawns in our grid. Init their values.
		do {
			valid_spawn_positions = true;
			for (int i = 0; i < numSpawns; i++) {
				spawn_positions[i] = FIntPoint(FMath::RandRange(0, gridSize.X - 1), FMath::RandRange(1, gridSize.Y - 1));
				spawn_directions[i] = FMath::RandRange(0, 3);
				if (grid[spawn_positions[i].Y * gridSize.X + spawn_positions[i].X] != kCellType_Normal) valid_spawn_positions = false;
			}
		} while (valid_spawn_positions == false);

		//Check Spawns Distances
		for (int a = 0; a < numSpawns; a++) {
			for (int b = 0; b < numSpawns; b++) {
				if (a != b) {
					//squared_distance=((b.x-a.x)²+(b.y-a.y)²)
					int squared_distance = (spawn_positions[b].X - spawn_positions[a].X) * (spawn_positions[b].X - spawn_positions[a].X);
					squared_distance += (spawn_positions[b].Y - spawn_positions[a].Y) * (spawn_positions[b].Y - spawn_positions[a].Y);
					if (squared_distance < (spawnMinDistance * spawnMinDistance)) valid_spawn_distances = false;
				}
			}
		}
	} while (valid_spawn_distances == false);

	//Set Spawners for each walker.
	for (int i = 0; i < numSpawns; i++) {
		//Spawn Generation.
		for (int j = 0; j < spawnSize; j++) {
			grid[spawn_positions[i].Y * gridSize.X + spawn_positions[i].X] = kCellType_Spawn;
			bool is_valid_spawn = false;
			while (is_valid_spawn == false) {
				bool is_valid_dir = false;
				//Check New Position for the walker
				FIntPoint new_position = spawn_positions[i];
				switch (spawn_directions[i])
				{
				case 0:
					new_position.Y += 1;
					break;
				case 1:
					new_position.X += 1;
					break;
				case 2:
					new_position.Y -= 1;
					break;
				case 3:
					new_position.X -= 1;
					break;
				}
				is_valid_dir = (new_position.Y >= 0 && new_position.Y < gridSize.Y&&
					new_position.X >= 0 && new_position.X < gridSize.X);
				//If valid, change direction if possible. 
				if (is_valid_dir) {
					spawn_positions[i] = new_position;
					if (!(grid[spawn_positions[i].Y * gridSize.X + spawn_positions[i].X] == kCellType_Spawn)) {
						switch (spawn_directions[i])
						{
						case 0:
							new_position.X += 1;
							break;
						case 1:
							new_position.Y -= 1;
							break;
						case 2:
							new_position.X -= 1;
							break;
						case 3:
							new_position.Y += 1;
							break;
						}
						if (!(grid[new_position.Y * gridSize.X + new_position.X] == kCellType_Spawn)) {
							switch (spawn_directions[i])
							{
							case 0:
								spawn_directions[i] = 1;
								break;
							case 1:
								spawn_directions[i] = 2;
								break;
							case 2:
								spawn_directions[i] = 3;
								break;
							case 3:
								spawn_directions[i] = 0;
								break;
							}
						}
						is_valid_spawn = true;
					}
					//If not valid, change direction to allow walker to move.
				}
				else {
					switch (spawn_directions[i])
					{
					case 0:
						spawn_directions[i] = 1;
						break;
					case 1:
						spawn_directions[i] = 2;
						break;
					case 2:
						spawn_directions[i] = 3;
						break;
					case 3:
						spawn_directions[i] = 0;
						break;
					}
				}
			}
		}
	}
};

void AGrid::CreateGrid()
{
	Cells.Init(nullptr, GridSize.Y * GridSize.X);

	TArray<CellType> cellTypes; 
	if (!PerlinORWalker)
	{
		cellTypes = GenerateGridWalkers(GridSize, NumberOfWalkers, NumberOfIterations);
	}
	else
	{
		cellTypes = GenerateGridPerlin(GridSize, ObstaclePercentaje, ObstacleDiffusion);
	}

	GenerateGridSpawns(cellTypes, GridSize, NumberOfSpawns, SpawnSize, SpawnMinDistance);

	if (ActorToInstantiate)
	{
		for (int i = 0; i < GridSize.Y; ++i)
		{
			for (int j = 0; j < GridSize.X; ++j)
			{
				int index = j + GridSize.X * i;
				FVector position(j * SquareWidth, i * SquareWidth, 0.0f);
				Cells[index] = Cast<ACell>(SpawnItem(ActorToInstantiate, position));
				Cells[index]->Init(index, this);
				Cells[index]->SetType(cellTypes[index]);
				if (cellTypes[index] == kCellType_Wall) {
					Cells[index]->SetActorHiddenInGame(true);
				}
			}
		}
	}

	//Checks if not null
	if (CharacterToInstantiate)
	{
		//Checks if the position is less than the array size
		if (spawn_position_ < Cells.Num())
		{
			//New character position
			FVector new_pos = Cells[spawn_position_]->GetActorLocation();
			new_pos.Z += 100.0f;

			//Spawn character and set it's cell pointer
			ACustomCharacter* cchar = Cast<ACustomCharacter>(SpawnItem(CharacterToInstantiate, new_pos));
			if (cchar)
			{
				cchar->InitPlayer(Cells[spawn_position_]);
			}


			//Set the character pointer on the character cell.
			Cells[spawn_position_]->SetCharacterPointer(cchar);
		}
	}
}



