// Fill out your copyright notice in the Description page of Project Settings.

#include "Grid/Grid.h"
#include "Grid/Cell.h"
#include "Character/CustomCharacter.h"
#include "DrawDebugHelpers.h"
#include "Containers/Array.h"
#include "Engine/World.h"
#include <list>

// Sets default values
AGrid::AGrid()
{
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();
}

void AGrid::Init()
{
	InitGrid();
	ConnectCells();
	GenerateObstacles();
	GenerateSpawns();
	SpawnCharacter();
}

int AGrid::CalculateManhattanDistance(ACell* a, ACell* b)
{
	if (a == nullptr || b == nullptr)
	{
		return 0;
	}
	return (abs(a->GetRow() - b->GetRow()) + abs(a->GetColumn() - b->GetColumn()));
}

float AGrid::GetCellSize()
{
	return CellSize;
}

bool AGrid::IsValidID(ACell* cell)
{
	if (cell) {
		return IsValidID(cell->GetID());
	}

	return false;
}

bool AGrid::IsValidID(int id)
{
	if (id >= 0 && id < (GridSize.X * GridSize.Y))
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
	return (row >= 0 && row < GridSize.Y&& col >= 0 && col < GridSize.X);
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
			TArray<ACell*> path = FindPath(tmp, new_cell);

			character->path_cells_ = path;

			tmp->SetCharacterPointer(nullptr);
			tmp->SetType(CellType::kCellType_Normal);
		}
	}
}

float AGrid::CalculateDistance(ACell* a, ACell* b)
{
	return (float)((a->GetColumn() - b->GetColumn()) * (a->GetColumn() - b->GetColumn()) + (a->GetRow() - b->GetRow()) * (a->GetRow() - b->GetRow()));
}

TArray<ACell*> AGrid::FindPath(ACell* start, ACell* finish)
{
	// Puts the start cell with initial values and sets as current
	start->localGoal = 0.0f;
	start->globalGoal = CalculateDistance(start, finish);
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
		for (auto cellNeighbour : cellCurrent->GetNeighbours())
		{
			if (!cellNeighbour->bvisited && cellNeighbour->GetType() == kCellType_Normal && cellNeighbour->GetCharacterPointer() == nullptr)
			{
				notTestedCells.push_back(cellNeighbour); // puts in the vector if it's not a wall and has not been visited

				// The potential lowest distance
				float fPossiblyLowerGoal = cellCurrent->localGoal + CalculateDistance(cellCurrent, cellNeighbour);

				// Sets the path if the potential distance is smaller than the neighbours'
				if (fPossiblyLowerGoal < cellNeighbour->localGoal)
				{
					cellNeighbour->SetParent(cellCurrent);
					cellNeighbour->localGoal = fPossiblyLowerGoal;

					// updates the neighbour score as the path length has been changed
					cellNeighbour->globalGoal = cellNeighbour->localGoal + CalculateDistance(cellNeighbour, finish);
				}
			}
		}
	}



	TArray<ACell*> cellPath;

	if (finish->GetParent() != nullptr)
	{
		if (finish != nullptr)
		{
			ACell* cell = finish;
			while (cell->GetParent() != nullptr)
			{
				cellPath.Push(cell);

				cell = cell->GetParent();
			}
		}
	}

	for (auto cell : Cells)
	{
		cell->localGoal = INFINITY;
		cell->globalGoal = INFINITY;
		cell->SetParent(nullptr);
		cell->bvisited = false;
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

static TArray<CellType> GenerateObstacles_CellularAutomata(FIntPoint grid_size, float obstacle_percentage, int neighbours_conversion_ratio, int iterations) {

	// Init CellType Values. 
	TArray<CellType> cell_types;
	cell_types.Init(kCellType_Void, grid_size.X * grid_size.Y);

	//Temporal Array to check different Grid Generations.
	TArray<CellType> new_cell_types;
	new_cell_types.Init(kCellType_Void, grid_size.X * grid_size.Y);

	float random_value = 0.0f;

	for (int value = 0; value < (grid_size.Y * grid_size.X); ++value) {
		random_value = FMath::FRandRange(0.0f, 1.0f);
		if (random_value > obstacle_percentage) {
			cell_types[value] = CellType::kCellType_Normal;
		}
		else {
			cell_types[value] = CellType::kCellType_Wall;
		}
	}

	for (int value = 0; value < iterations; ++value) {
		//Count Number of Obstacles around each Cell.
		for (int r = 0; r < grid_size.Y; ++r) {
			for (int c = 0; c < grid_size.X; ++c) {
				int neighbours_obstacle_counter = 0;
				int index = c + grid_size.X * r;
				//North
				if (r == grid_size.Y - 1) {
					neighbours_obstacle_counter += 3;
				}
				else {
					if (cell_types[index + grid_size.X] == CellType::kCellType_Wall)neighbours_obstacle_counter++;
					//NorthWest
					if (c == 0) {
						neighbours_obstacle_counter++;
					}
					else {
						if (cell_types[index + grid_size.X - 1] == CellType::kCellType_Wall)neighbours_obstacle_counter++;
					}
					//NorthEast
					if (c == grid_size.X - 1) {
						neighbours_obstacle_counter++;
					}
					else {
						if (cell_types[index + grid_size.X + 1] == CellType::kCellType_Wall)neighbours_obstacle_counter++;
					}
				}
				//South
				if (r == 0) {
					neighbours_obstacle_counter += 3;
				}
				else {
					if (cell_types[index - grid_size.X] == CellType::kCellType_Wall)neighbours_obstacle_counter++;
					//SouthWest
					if (c == 0) {
						neighbours_obstacle_counter++;
					}
					else {
						if (cell_types[index - grid_size.X - 1] == CellType::kCellType_Wall)neighbours_obstacle_counter++;
					}
					//SouthEast
					if (c == grid_size.X - 1) {
						neighbours_obstacle_counter++;
					}
					else {
						if (cell_types[index - grid_size.X + 1] == CellType::kCellType_Wall)neighbours_obstacle_counter++;
					}
				}
				//West
				if (c == 0) {
					neighbours_obstacle_counter++;
				}
				else {
					if (cell_types[index - 1] == CellType::kCellType_Wall)neighbours_obstacle_counter++;
				}
				//East
				if (c == grid_size.X - 1) {
					neighbours_obstacle_counter++;
				}
				else {
					if (cell_types[index + 1] == CellType::kCellType_Wall)neighbours_obstacle_counter++;
				}

				new_cell_types[index] = cell_types[index];
				//Here you should have all the neighbours calculated.
				if (neighbours_obstacle_counter < neighbours_conversion_ratio) {
					new_cell_types[index] = CellType::kCellType_Normal;
				}
				if (neighbours_obstacle_counter > neighbours_conversion_ratio) {
					new_cell_types[index] = CellType::kCellType_Wall;
				}
			}
		}
		cell_types = new_cell_types;
	}

	return cell_types;
}

static TArray<CellType> GenerateObstacles_Walkers(FIntPoint gridSize, int walkersNum, int iterations) {

	//Set CellType Array
	TArray<CellType> cell_types;
	cell_types.Init(CellType::kCellType_Wall, gridSize.X * gridSize.Y);

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
			is_valid_dir = (new_position.Y >= 0 && new_position.Y < gridSize.Y&&
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

static TArray<CellType> GenerateObstacles_Perlin(FIntPoint size, float obstaclePercentaje, FVector2D obstacleDiffusion) {
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

//Return all conected (This could be way easier if we worked with the cells instead, but I wanted to complicate things for myself.)
static TArray<int> GetWalkableAreaIndices(TArray<CellType> cell_types, TArray<bool>& checks, int cell_index, FIntPoint grid_size) {

	TArray<int> area_values;
	if (cell_types[cell_index] == CellType::kCellType_Normal && checks[cell_index] == false)
	{
		area_values.Push(cell_index);
		checks[cell_index] = true;
	}

	int row = cell_index / grid_size.X;
	int col = cell_index % grid_size.X;

	//North
	if (row < (grid_size.X - 1)) {
		if (cell_types[cell_index + grid_size.X] == CellType::kCellType_Normal && checks[cell_index + grid_size.X] == false) {
			area_values.Append(GetWalkableAreaIndices(cell_types, checks, cell_index + grid_size.X, grid_size));
		}
	}

	//South
	if (row > 0) {
		if (cell_types[cell_index - grid_size.X] == CellType::kCellType_Normal && checks[cell_index - grid_size.X] == false) {
			area_values.Append(GetWalkableAreaIndices(cell_types, checks, cell_index - grid_size.X, grid_size));
		}
	}

	//East
	if (col < (grid_size.Y - 1)) {
		if (cell_types[cell_index + 1] == CellType::kCellType_Normal && checks[cell_index + 1] == false) {
			area_values.Append(GetWalkableAreaIndices(cell_types, checks, cell_index + 1, grid_size));
		}
	}

	//West
	if (col > 0) {
		if (cell_types[cell_index - 1] == CellType::kCellType_Normal && checks[cell_index - 1] == false) {
			area_values.Append(GetWalkableAreaIndices(cell_types, checks, cell_index - 1, grid_size));
		}
	}
	return area_values;
}

static void RemoveUnconnectedAreas(TArray<CellType>& cell_types, FIntPoint grid_size) {

	TArray<bool> cell_area_check;
	TArray<TArray<int>> cell_areas;

	cell_area_check.Init(false, grid_size.X * grid_size.Y);

	for (int idx = 0; idx < grid_size.X * grid_size.Y; idx++) {
		if (cell_types[idx] == CellType::kCellType_Wall) {
			cell_area_check[idx] = true;
		}
		else {
			if (cell_area_check[idx] == false) {
				TArray<int> area_values;
				area_values = GetWalkableAreaIndices(cell_types, cell_area_check, idx, grid_size);
				cell_areas.Push(area_values);
			}
		}
	}

	int biggest_area_index = 0;
	int biggest_area_size = 0;

	for (int i = 0; i < cell_areas.Num(); i++) {
		if (cell_areas[i].Num() > biggest_area_size) {
			biggest_area_index = i;
			biggest_area_size = cell_areas[i].Num();
		}
	}

	for (int area = 0; area < cell_areas.Num(); area++) {
		for (int idx = 0; idx < cell_areas[area].Num(); idx++) {
			if (area != biggest_area_index) cell_types[(cell_areas[area])[idx]] = CellType::kCellType_Void;
		}
	}
};

void AGrid::InitGrid() {

	Cells.Init(nullptr, GridSize.Y * GridSize.X);

	for (int r = 0; r < GridSize.Y; ++r) {
		for (int c = 0; c < GridSize.X; ++c) {

			int cell_index = c + GridSize.X * r;
			FVector cell_position = FVector(c * CellSize, r * CellSize, 0.0f);
			FRotator cell_rotation = FRotator(0.0f);

			Cells[cell_index] = Cast<ACell>(GetWorld()->SpawnActor<AActor>(CellToInstantiate, cell_position, cell_rotation));

			Cells[cell_index]->SetGridPointer(this);
			Cells[cell_index]->SetID(cell_index);
			Cells[cell_index]->SetMeshSize(CellSize);
		}
	}
}

void AGrid::GenerateObstacles()
{
	TArray<CellType> cellTypes;

	switch (GridGenerationMethod)
	{
	case kGridGeneration_None:
		cellTypes.Init(kCellType_Void, GridSize.X * GridSize.Y);
		break;
	case kGridGeneration_CelularAutomata:
		cellTypes = GenerateObstacles_CellularAutomata(GridSize, InitialObstaclePercentaje, NeighboursToTileConvertion, NumberOfTransitions);
		break;
	case kGridGeneration_PerlinNoise:
		cellTypes = GenerateObstacles_Perlin(GridSize, ObstaclePercentaje, ObstacleDiffusion);
		break;
	case kGridGeneration_Walkers:
		cellTypes = GenerateObstacles_Walkers(GridSize, NumberOfWalkers, IterationsPerWalker);
		break;
	default:
		cellTypes.Init(kCellType_Void, GridSize.X * GridSize.Y);
		break;
	}

	RemoveUnconnectedAreas(cellTypes, GridSize);

	for (int value = 0; value < Cells.Num(); value++) {
		Cells[value]->SetType(cellTypes[value]);
		Cells[value]->SetCellMaterial();
	}
}

void AGrid::ConnectCells()
{
	for (int x = 0; x < GridSize.X; x++)
	{
		for (int y = 0; y < GridSize.Y; y++)
		{
			TArray<ACell*> cell_neighbours;
			// Make sure that the cells are inside the bounds
			if (y > 0)
				cell_neighbours.Push(Cells[(y - 1) * GridSize.X + (x + 0)]);
			if (y < GridSize.Y - 1)
				cell_neighbours.Push(Cells[(y + 1) * GridSize.X + (x + 0)]);
			if (x > 0)
				cell_neighbours.Push(Cells[(y + 0) * GridSize.X + (x - 1)]);
			if (x < GridSize.X - 1)
				cell_neighbours.Push(Cells[(y + 0) * GridSize.X + (x + 1)]);

			Cells[y * GridSize.X + x]->SetNeighbours(cell_neighbours);
		}
	}
}

static void ExpandSpawns(FGridSpawn& spawn, ACell* cell_ref) {
	//spawn.SpawnCells.Push(cell_ref);
	TArray<ACell*> n_cells;
	for (int n_cell = 0; n_cell < cell_ref->GetNeighbours().Num(); n_cell++) {
		bool valid_neighbour = true;
		//Check that the neighbour cells are not inside our spawn cell array already.
		for (int s_cell = 0; s_cell < spawn.SpawnCells.Num(); s_cell++) {
			if (spawn.SpawnCells[s_cell]->GetID() == cell_ref->GetNeighbours()[n_cell]->GetID()) {
				valid_neighbour = false;
			}
		}
		//Add valid neighbours to the spawn cells arrays.
		if (valid_neighbour && cell_ref->GetNeighbours()[n_cell]->GetType() == kCellType_Normal && spawn.SpawnCells.Num() < spawn.SpawnSize) {
			spawn.SpawnCells.Push(cell_ref->GetNeighbours()[n_cell]);
			n_cells.Push(cell_ref->GetNeighbours()[n_cell]);
		}
	}
	//If needed, add more cells to the spawn array.
	for (int cell_idx = 0; cell_idx < n_cells.Num(); cell_idx++) {
		ExpandSpawns(spawn, n_cells[cell_idx]);
	}
}

void AGrid::GenerateSpawns() {
	
	bool valid_spawn_distances = true;
	TArray<FIntPoint> spawn_positions;
	spawn_positions.Init(FIntPoint(0, 0), Spawns.Num());

	do {
		valid_spawn_distances = true;
		//Validate First Spawn Position in our grid.
		for (int index = 0; index < Spawns.Num(); index++) {
			bool is_valid_position = false;
			do {
				spawn_positions[index] = FIntPoint(FMath::RandRange(0, GridSize.X - 1), FMath::RandRange(0, GridSize.Y - 1));
				if (Cells[spawn_positions[index].Y * GridSize.X + spawn_positions[index].X]->GetType() == kCellType_Normal) {
					is_valid_position = true;
				}
			} while (is_valid_position == false);
		}
		//Check Distances Between Spawns.
		for (int a = 0; a < Spawns.Num(); a++) {
			for (int b = 0; b < Spawns.Num(); b++) {
				if (a != b) {
					//squared_distance=((b.x-a.x)²+(b.y-a.y)²)
					int squared_distance = (spawn_positions[b].X - spawn_positions[a].X) * (spawn_positions[b].X - spawn_positions[a].X);
					squared_distance += (spawn_positions[b].Y - spawn_positions[a].Y) * (spawn_positions[b].Y - spawn_positions[a].Y);
					if (squared_distance < (SpawnMinDistance * SpawnMinDistance)) valid_spawn_distances = false;
				} 
			}
		}
	} while (valid_spawn_distances == false);

	for (int index = 0; index < Spawns.Num(); index++) {

		Spawns[index].SpawnSize = 0;

		//Calculate Spawn Size Before Expanding Cells.
		for (int char_idx = 0; char_idx < Spawns[index].SpawnCharacters.Num(); char_idx++) {
			Spawns[index].SpawnSize += Spawns[index].SpawnCharacters[char_idx].NumberOfCharacters;
		}

		//Set the initial Cell for the Spawn.
		Spawns[index].SpawnCells.Push(Cells[spawn_positions[index].Y * GridSize.X + spawn_positions[index].X]);
		//Expand the Spawn Until it covers as many cells as they need.
		ExpandSpawns(Spawns[index], Cells[spawn_positions[index].Y * GridSize.X + spawn_positions[index].X]);

		//Set Material for the Spawn Cells.
		for (int value = 0; value < Spawns[index].SpawnCells.Num(); value++) {
			Spawns[index].SpawnCells[value]->SetCellSpawnMaterial(Spawns[index].SpawnTeam);
		}
	}
}

void AGrid::SpawnCharacter() {

	//Check Our Spawns.
	if (Spawns.Num() > 0) {
		//If we have Spawns we assign the characters to the spawn cells.
		for (int spawn_idx = 0; spawn_idx < Spawns.Num(); spawn_idx++) {

			//Make An array of CharacterTypes that are going to be instaciated inside our game.
			TArray<ACustomCharacter*> characters;

			for (int char_idx = 0; char_idx < Spawns[spawn_idx].SpawnCharacters.Num(); char_idx++) {
				for(int n_idx = 0; n_idx < Spawns[spawn_idx].SpawnCharacters[char_idx].NumberOfCharacters; n_idx++) {
					ACustomCharacter* character = Cast<ACustomCharacter>(GetWorld()->SpawnActor<AActor>(
						Spawns[spawn_idx].SpawnCharacters[char_idx].CharacterToInstantiate,
						FVector::ZeroVector,
						FRotator::ZeroRotator
					));
					characters.Push(character);
				}
			}

			for (int spawn_cell = 0; spawn_cell < Spawns[spawn_idx].SpawnCells.Num(); spawn_cell++) {
				//Set character position
				FVector character_position = Spawns[spawn_idx].SpawnCells[spawn_cell]->GetActorLocation();
				FRotator character_rotation = FRotator(0.0f);

				//Spawn character and set it.
				if (characters.Num() > 0 && characters.Num() > spawn_cell && characters[spawn_cell] != nullptr) {
					characters[spawn_cell]->SetActorLocation(character_position);
					characters[spawn_cell]->SetActorRotation(character_rotation);
					characters[spawn_cell]->InitPlayer(Spawns[spawn_idx].SpawnCells[spawn_cell]);
					characters[spawn_cell]->SetCharacterTeam(Spawns[spawn_idx].SpawnTeam);
					//character->UpdateMaterial();
				}
				//Set the character pointer on the character cell.
				Spawns[spawn_idx].SpawnCells[spawn_cell]->SetCharacterPointer(characters[spawn_cell]);
				Spawns[spawn_idx].SpawnCells[spawn_cell]->SetType(CellType::kCellType_Spawn);
			}
		}
	}
	/* Spawn a Random Character into the Grid.
	else {
		//Set a proper position for our character.
		int character_index = FMath::FRandRange(0, Cells.Num() - 1);
		bool valid_position = false;

		int repetitions = 0;

		//IMPORTANT: Check if the cell is already asigned to another character
		while (valid_position == false || repetitions > (Cells.Num() * 2)) {
			if (Cells[character_index]->GetType() == CellType::kCellType_Normal || Cells[character_index]->GetType() == CellType::kCellType_Spawn) {
				valid_position = true;
			}
			else {
				character_index = FMath::FRandRange(0, Cells.Num() - 1);
			}
		}

		//New character position
		FVector new_pos = Cells[character_index]->GetActorLocation();
		new_pos.Z += 50.0f;
		FRotator character_rotation = FRotator(0.0f);


		//Spawn character and set it's cell pointer
		ACustomCharacter* cchar = Cast<ACustomCharacter>(GetWorld()->SpawnActor<AActor>(CharacterToInstantiate, new_pos, character_rotation));
		if (cchar)
		{
			cchar->InitPlayer(Cells[character_index]);
		}

		//Set the character pointer on the character cell.
		Cells[character_index]->SetCharacterPointer(cchar);
		Cells[character_index]->SetType(CellType::kCellType_Normal);
	}
	*/
}

void AGrid::HighlightMoveCells(TArray<ACell*> cell_array)
{
	for (auto c : cell_array) {
		c->HighlightCell(CellMaterial::kCellMaterial_MoveHighlight);
	}
}

void AGrid::HighlightAttackCells(TArray<ACell*> cell_array)
{
	for (auto c : cell_array) {
		c->HighlightCell(CellMaterial::kCellMaterial_AttackHighlight);
	}
}

void AGrid::UnhighlightCells(TArray<ACell*> cell_array)
{
	for (auto c : cell_array) {
		c->HighlightCell(CellMaterial::kCellMaterial_Default);
	}
	//Set Material for the Spawn Cells.
	for (int num_spawns = 0; num_spawns < Spawns.Num(); num_spawns++) {
		for (int num_cells = 0; num_cells < Spawns[num_spawns].SpawnCells.Num(); num_cells++) {
			Spawns[num_spawns].SpawnCells[num_cells]->SetCellSpawnMaterial(Spawns[num_spawns].SpawnTeam);
		}
	}



}

ACell* AGrid::GetCellByID(int id)
{
	if (IsValidID(id)) {
		return Cells[id];
	}
	return nullptr;
}


