// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.generated.h"


class ACell;
class ACustomCharacter;



UCLASS()
class TACTICS_API AGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Returns the size of the cell
	FVector GetCellSize();

	// Checkers
	bool IsValidID(ACell* cell);
	bool isWall(ACell* cell);
	bool IsValidRowCol(int row, int col);


	// Transformers
	void IndexToRowCol(int* row, int* col, ACell* origin);

	// Return the ID of the north, east, south or west cells if is walkable
	int North(ACell* origin);
	int South(ACell* origin);
	int East(ACell* origin);
	int West(ACell* origin);

	void MoveCharacterToCell(ACustomCharacter*, ACell*);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Instantiation, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> ActorToInstantiate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Instantiation, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AActor> CharacterToInstantiate;

	UPROPERTY(EditAnywhere, Category = Instantiation)
		int spawn_position_;

	AActor* SpawnItem(UClass* ItemToSpawn, FVector& Position);
	
	void CreateGrid();

	/* Each cell connects with their neighbours via the vector */
	void ConnectCells();

	float RoomLength;
	float RoomWidth;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Size)
		float SquareWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Size)
		FIntPoint GridSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		TArray<ACell*> Cells;

	UPROPERTY(EditAnywhere, Category = GridGeneration)
		bool PerlinORWalker; //true Perlin - false Walker

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProceduralWalkers)
		int NumberOfWalkers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProceduralWalkers)
		int NumberOfIterations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Obstacles)
		float ObstaclePercentaje; //between 0.0 and 1.0

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Obstacles)
		FVector2D ObstacleDiffusion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
		int NumberOfSpawns;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
		int SpawnSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
		int SpawnMinDistance;
};
