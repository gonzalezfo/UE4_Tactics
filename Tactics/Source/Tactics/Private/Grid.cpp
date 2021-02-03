// Fill out your copyright notice in the Description page of Project Settings.

#include "Grid.h"
#include "Cell.h"
#include "DrawDebugHelpers.h"

// Sets default values
AGrid::AGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GridSize.X = 5;
	GridSize.Y = 5;

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


void AGrid::IndexToRowCol(int* row, int* col, ACell* origin)
{
	if (!IsValidID(origin))
	{
		return;
	}

	*row = origin->GetID() / GridSize.X;
	*col = origin->GetID() % GridSize.X;
}


AActor* AGrid::SpawnItem(UClass* ItemToSpawn, FVector& Position)
{
	FRotator rotation (0.0f);

	return GetWorld()->SpawnActor<AActor>(ItemToSpawn, Position, rotation);
}

static TArray<CellType> GenerateGridTypes(FIntPoint size, float obstaclePercentaje, FVector2D obstacleDiffusion) {
	TArray<CellType> cell_types;
	cell_types.Init(kCellType_Void, size.X * size.Y);

	float pseudo_seed = FMath::FRandRange(-1.0f, 1.0f);

	for (int i = 0; i < size.Y; ++i) {
		for (int j = 0; j < size.X; ++j) {
			int index = i * size.X + j;
			FVector location = { i * obstacleDiffusion.X , j * obstacleDiffusion.Y ,  pseudo_seed };
			float value = FMath::PerlinNoise3D(location) + 1.0f;
			value = value / 2.0f; //renormalize between 0 and 1
			if (value < obstaclePercentaje) {
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

	TArray<CellType> cellTypes = GenerateGridTypes(GridSize, ObstaclePercentaje, ObstacleDiffusion);

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
				FVector newPos = position + FVector(0.0f, 0.0f, 1.0f);
				SpawnItem(ActorToInstantiate, newPos);
			}
		}
	}
}



