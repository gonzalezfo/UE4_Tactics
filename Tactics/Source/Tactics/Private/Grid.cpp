// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/Grid.h"
#include "../Public/Cell.h"
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
	TopLeft = FVector(0.0f);
	BottomRight = FVector(RoomWidth, RoomLength, 0.0f);
}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();	

	CreateGrid();
	PlacePointsOnGrid();
}

// Called every frame
void AGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AGrid::GetCellWidth()
{
	return SquareWidth;
}

void AGrid::SpawnItem(UClass* ItemToSpawn, FVector& Position)
{
	FRotator rotation (0.0f);

	GetWorld()->SpawnActor<AActor>(ItemToSpawn, Position, rotation);
}

void AGrid::CreateGrid()
{
	for (int i = 0; i <= GridSizeX; ++i)
	{
		for (int j = 0; j <= GridSizeY; ++j)
		{
			
		}
	}
}

FVector AGrid::GetMidPointInGrid(const FVector& UpperLeft, const FVector& LowerRight)
{
	float MidX = UpperLeft.X + (SquareWidth * 0.5f);
	float MidY = LowerRight.Y - (SquareWidth * 0.5f);

	return FVector(MidX, MidY, 0.0f);
}

void AGrid::PlacePointsOnGrid()
{
	for (int i = 0; i < GridSizeX; ++i)
	{
		for (int j = 0; j < GridSizeY; ++j)
		{
			FVector UpperLeft(i * SquareWidth, j * SquareWidth, GridHeight);
			FVector LowerRight(i * SquareWidth + SquareWidth, j * SquareWidth + SquareWidth, GridHeight);
			FVector CenterPointInSquare = GetMidPointInGrid(UpperLeft, LowerRight);
			
			DrawDebugCircle(GetWorld(), CenterPointInSquare, 25.f, 48, FColor::Red, true, -1.0f, 0, 2.5f, FVector(0.0f, 1.0f, 0.0f), FVector(1.0f, 0.0f, 0.0f), true);

			SpawnItem(ActorToInstantiate, CenterPointInSquare);
		}
	}
}

