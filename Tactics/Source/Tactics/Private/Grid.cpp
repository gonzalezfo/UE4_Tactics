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

	GridSizeX = 5;
	GridSizeY = 5;

	SquareWidth = 200.0f;

	RoomLength = SquareWidth * GridSizeY;
	RoomWidth = SquareWidth* GridSizeX;
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
	if (cell->GetID() >= 0 && cell->GetID() < (GridSizeX * GridSizeY))
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
	return (row >= 0 && row < GridSizeY && col >= 0 && col < GridSizeX);
}

int AGrid::North(ACell* origin)
{
	if (!IsValidID(origin))
	{
		return -1;
	}

	int const index = origin->GetID() - GridSizeX;

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

	int const index = origin->GetID() + GridSizeX;

	if (index > GridSizeX * GridSizeY)
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

	if (col == GridSizeX - 1)
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

	*row = origin->GetID() / GridSizeX;
	*col = origin->GetID() % GridSizeX;
}


AActor* AGrid::SpawnItem(UClass* ItemToSpawn, FVector& Position)
{
	FRotator rotation (0.0f);

	return GetWorld()->SpawnActor<AActor>(ItemToSpawn, Position, rotation);
}


void AGrid::CreateGrid()
{
	Cells.Init(nullptr, GridSizeY * GridSizeX);

	if (ActorToInstantiate)
	{
		for (int i = 0; i < GridSizeY; ++i)
		{
			for (int j = 0; j < GridSizeX; ++j)
			{
				int index = j + GridSizeX * i;
				FVector position(j * SquareWidth, i * SquareWidth, 0.0f);
				Cells[index] = Cast<ACell>(SpawnItem(ActorToInstantiate, position));
				Cells[index]->Init(index, this);
				Cells[index]->SetType(kCellType_Normal);
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



