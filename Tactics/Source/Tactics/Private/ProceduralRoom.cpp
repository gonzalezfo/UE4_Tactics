// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralRoom.h"
#include "..\Public\ProceduralRoom.h"
#include "DrawDebugHelpers.h"

// Sets default values
AProceduralRoom::AProceduralRoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorComponent"));
	SetRootComponent(Floor);

	GridSizeX = 5;
	GridSizeY = 5;
	GridHeight = 1.0f;
	SquareWidth = 200.0f;
	RoomLength = SquareWidth * GridSizeY;
	RoomWidth = SquareWidth* GridSizeX;
	TopLeft = FVector(0.0f);
	BottomRight = FVector(RoomWidth, RoomLength, 0.0f);
}

// Called when the game starts or when spawned
void AProceduralRoom::BeginPlay()
{
	Super::BeginPlay();	

	CreateGrid();
	PlacePointsOnGrid();
}

// Called every frame
void AProceduralRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProceduralRoom::SpawnItem(UClass* ItemToSpawn, FVector& Position)
{

	float Yaw = FMath::FRandRange(0.0f, 360.0f);

	FRotator rotation (0.0f, Yaw, 0.0f);

	GetWorld()->SpawnActor<AActor>(ItemToSpawn, Position, rotation);
}

void AProceduralRoom::CreateGrid()
{
	for (int i = 0; i <= GridSizeX; ++i)
	{
		FVector Start = TopLeft + FVector(i * SquareWidth, 0.0f, GridHeight);
		FVector End = Start + FVector(0.0f, RoomWidth, GridHeight);
		DrawDebugLine(GetWorld(), Start, End, FColor::Blue, true);
	}

	for (int i = 0; i <= GridSizeY; ++i)
	{
		FVector Start = TopLeft + FVector(0.0, i * SquareWidth, GridHeight);
		FVector End = Start + FVector(RoomLength, 0.0f, GridHeight);
		DrawDebugLine(GetWorld(), Start, End, FColor::Blue, true);
	}
}

FVector AProceduralRoom::GetMidPointInGrid(const FVector& UpperLeft, const FVector& LowerRight)
{
	float MidX = UpperLeft.X + (SquareWidth * 0.5f);
	float MidY = LowerRight.Y - (SquareWidth * 0.5f);

	return FVector(MidX, MidY, 0.0f);
}

void AProceduralRoom::PlacePointsOnGrid()
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

