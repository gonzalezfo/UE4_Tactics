#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.generated.h"

class ACell;
class ACustomCharacter;

UENUM(BlueprintType)
enum GridGenerationType {
	kGridGeneration_None = 0				UMETA(DisplayName = "None"),
	kGridGeneration_CelularAutomata = 1		UMETA(DisplayName = "Celular Automata"),
	kGridGeneration_PerlinNoise = 2			UMETA(DisplayName = "Perlin Noise"),
	kGridGeneration_Walkers = 3				UMETA(DisplayName = "Walkers"),
};

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

	// Returns the size of the cell
	float GetCellSize();

	// Checkers
	bool IsValidID(ACell* cell);
	bool IsValidID(int id);
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

	TArray<ACell*> FindPath(ACell* start, ACell* finish);

	void HighlightCells(TArray<ACell*>);
	void UnhighlightCells(TArray<ACell*>);

	ACell* GetCellByID(int id);

private:

	void InitGrid();
	void ConnectCells();
	void GenerateObstacles();
	void SpawnCharacter();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<ACell> CellToInstantiate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AActor> CharacterToInstantiate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
		FIntPoint GridSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid", meta = (ClampMin = "1"))
		int CellSize = 100;

	UPROPERTY()
		TArray<ACell*> Cells;

	UPROPERTY(EditAnywhere, Category = "Grid")
		TEnumAsByte<GridGenerationType> GridGenerationMethod;

	UPROPERTY(EditAnywhere, Category = "Grid|Celular Automata Generation", meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float InitialObstaclePercentaje;

	UPROPERTY(EditAnywhere, Category = "Grid|Celular Automata Generation", meta = (ClampMin = "0", ClampMax = "8"))
		int NeighboursToTileConvertion;

	UPROPERTY(EditAnywhere, Category = "Grid|Celular Automata Generation", meta = (ClampMin = "1"))
		int NumberOfTransitions = 1;

	UPROPERTY(EditAnywhere, Category = "Grid|Perlin Noise Generation", meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float ObstaclePercentaje;

	UPROPERTY(EditAnywhere, Category = "Grid|Perlin Noise Generation", meta = (ClampMin = "0.0", ClampMax = "1.0"))
		FVector2D ObstacleDiffusion;

	UPROPERTY(EditAnywhere, Category = "Grid|Walkers Generation", meta = (ClampMin = "0"))
		int NumberOfWalkers = 1;

	UPROPERTY(EditAnywhere, Category = "Grid|Walkers Generation", meta = (ClampMin = "0"))
		int IterationsPerWalker = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Spawns", meta = (ClampMin = "0", ClampMax = "10"))
		int NumberOfSpawns;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Spawns", meta = (ClampMin = "0", ClampMax = "10"))
		int SpawnSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Spawns")
		int SpawnMinDistance;
};
