#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.generated.h"

class ACell;
class ACustomCharacter;

UENUM(BlueprintType)
enum EGridGenerationType {
	kGridGeneration_None = 0				UMETA(DisplayName = "None"),
	kGridGeneration_CelularAutomata = 1		UMETA(DisplayName = "Celular Automata"),
	kGridGeneration_PerlinNoise = 2			UMETA(DisplayName = "Perlin Noise"),
	kGridGeneration_Walkers = 3				UMETA(DisplayName = "Walkers"),
};

UENUM(BlueprintType)
enum ESpawnTeam {
	kSpawnTeam_Player = 0			UMETA(DisplayName = "Player"),
	kSpawnTeam_Team_1 = 1			UMETA(DisplayName = "Team 1"),
	kSpawnTeam_Team_2 = 2			UMETA(DisplayName = "Team 2"),
	kSpawnTeam_Team_3 = 3			UMETA(DisplayName = "Team 3"),
	/*
	kSpawnTeam_Team_4 = 4			UMETA(DisplayName = "Team 4"),
	kSpawnTeam_Team_5 = 5			UMETA(DisplayName = "Team 5"),
	kSpawnTeam_Team_6 = 6			UMETA(DisplayName = "Team 6"),
	kSpawnTeam_Team_7 = 7			UMETA(DisplayName = "Team 7"),
	kSpawnTeam_Team_8 = 8			UMETA(DisplayName = "Team 8"),
	kSpawnTeam_Team_9 = 9			UMETA(DisplayName = "Team 9"),
	*/
	kSpawnTeam_ERROR = -1			UMETA(DisplayName = "Invalid Team"),
};

USTRUCT(BlueprintType)
struct FSpawnCharacterComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		TSubclassOf<ACustomCharacter> CharacterToInstantiate;

	UPROPERTY(EditAnywhere, Category = "Character Data", meta = (ClampMin = "1", ClampMax = "10"))
		int NumberOfCharacters = 1;
};

USTRUCT(BlueprintType)
struct FGridSpawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Spawn Data")
		TEnumAsByte<ESpawnTeam> SpawnTeam;

	UPROPERTY(EditAnywhere, Category = "Spawn Data")
		TArray<FSpawnCharacterComponent> SpawnCharacters;

	TArray<ACell*> SpawnCells;

	int SpawnSize = 0;
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

	void Init();

	int CalculateManhattanDistance(ACell* a, ACell* b);

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

	void HighlightMoveCells(TArray<ACell*>);
	void HighlightAttackCells(TArray<ACell*>);
	void HighlightHealCells(TArray<ACell*>);
	void UnhighlightCells(TArray<ACell*>);

	ACell* GetCellByID(int id);
	float CalculateDistance(ACell* a, ACell* b);

	void SpawnCharacters();

private:

	void InitGrid();
	void ConnectCells();
	void GenerateObstacles();
	void GenerateSpawns();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<ACell> CellToInstantiate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
		FIntPoint GridSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid", meta = (ClampMin = "1"))
		int CellSize = 100;

	UPROPERTY()
		TArray<ACell*> Cells;

	UPROPERTY(EditAnywhere, Category = "Grid")
		TEnumAsByte<EGridGenerationType> GridGenerationMethod;

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
		TArray<FGridSpawn> Spawns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Spawns")
		int SpawnMinDistance;
};
