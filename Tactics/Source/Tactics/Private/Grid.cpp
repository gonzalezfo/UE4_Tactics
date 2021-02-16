// Fill out your copyright notice in the Description page of Project Settings.

#include "Grid.h"
#include "Cell.h"
#include "CustomCharacter.h"
#include "DrawDebugHelpers.h"
#include "Containers/Array.h"
#include "..\Public\Grid.h"
#include <list>

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
	ConnectCells();
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

TArray<ACell*> AGrid::FindPath(ACell* start, ACell* finish)
{
	auto distance = [](ACell* a, ACell* b)
	{
		return sqrtf((float)((a->col - b->col) * (a->col - b->col) + (a->row - b->row) * (a->row - b->row)));
	};

	// Puts the start cell with initial values and sets as current
	start->localGoal = 0.0f;
	start->globalGoal = distance(start, finish);
	ACell* cellCurrent = start;

	// Creation of the list to push all the non-visited cells
	std::list<ACell*> notTestedCells;
	notTestedCells.push_back(start);


	while (!notTestedCells.empty())
	{
		// Sort the non-visited cells by global param
		notTestedCells.sort([](const ACell* c1, const ACell* c2) { return c1->globalGoal < c2->globalGoal; });

		while (!notTestedCells.empty() != 0 && notTestedCells.front()->bvisited)
			notTestedCells.pop_front();

		// breaks the while if there are not cells left
		if (notTestedCells.empty())
			break;

		cellCurrent = notTestedCells.front();
		cellCurrent->bvisited = true; // sets the cell as visited, not to consider it again

		// Checks all the neighbour nodes of the current node
		for (auto cellNeighbour : cellCurrent->neighbours)
		{
			if (!cellNeighbour->bvisited && cellNeighbour->type != kCellType_Wall)
			{
				notTestedCells.push_back(cellNeighbour); // puts in the vector if it's not a wall and has not been visited

				// The potential lowest distance
				float fPossiblyLowerGoal = cellCurrent->localGoal + distance(cellCurrent, cellNeighbour);

				// Sets the path if the potential distance is smaller than the neighbours'
				if (fPossiblyLowerGoal < cellNeighbour->localGoal)
				{
					cellNeighbour->parent = cellCurrent;
					cellNeighbour->localGoal = fPossiblyLowerGoal;

					// updates the neighbour score as the path length has been changed
					cellNeighbour->globalGoal = cellNeighbour->localGoal + distance(cellNeighbour, finish);
				}
			}
		}
	}

	TArray<ACell*> cellPath;

	if (finish->parent != nullptr)
	{
		if (finish != nullptr)
		{
			ACell* cell = finish;
			while (cell->parent != nullptr)
			{
				cellPath.Push(cell);
				
				cell = cell->parent;
			}
		}
	}

	return cellPath;
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

	for (int i = 0; i < walkersNum; i++) {
		grid_walkers_positions[i] = FIntPoint(FMath::RandRange(0, gridSize.X - 1), FMath::RandRange(1, gridSize.Y - 1));
		grid_walkers_directions[i] = FMath::RandRange(0, 3);
		cell_types[grid_walkers_positions[i].Y * gridSize.X + grid_walkers_positions[i].X] = kCellType_Normal;
	}

	int iterator = 0;
	//Path Generation Loop.
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
			cell_types[grid_walkers_positions[i].Y * gridSize.X + grid_walkers_positions[i].X] = kCellType_Normal;
			grid_walkers_directions[i] = FMath::RandRange(0, 3);

		}
		iterator++;
	}

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

void AGrid::ConnectCells()
{
	for (int x = 0; x < GridSize.X; x++)
	{
		for (int y = 0; y < GridSize.Y; y++)
		{
			// Make sure that the cells are inside the bounds
			if (y > 0)
				Cells[y * GridSize.X + x]->neighbours.Push(Cells[(y - 1) * GridSize.X + (x + 0)]);
			if (y < GridSize.Y - 1)
				Cells[y * GridSize.X + x]->neighbours.Push(Cells[(y + 1) * GridSize.X + (x + 0)]);
			if (x > 0)
				Cells[y * GridSize.X + x]->neighbours.Push(Cells[(y + 0) * GridSize.X + (x - 1)]);
			if (x < GridSize.X - 1)
				Cells[y * GridSize.X + x]->neighbours.Push(Cells[(y + 0) * GridSize.X + (x + 1)]);
		}
	}
}



