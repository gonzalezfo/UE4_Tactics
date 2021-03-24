#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Grid/Grid.h"

#include "Cell.generated.h"

class ACustomCharacter;

UENUM(BlueprintType)
enum CellType {
	kCellType_Void = 0						UMETA(DisplayName = "VOID"),
	kCellType_Wall = 1						UMETA(DisplayName = "WALL"),
	kCellType_Normal = 2					UMETA(DisplayName = "NORMAL"),
	kCellType_Spawn = 3						UMETA(DisplayName = "SPAWN"),
};

UENUM(BlueprintType)
enum CellMaterial
{
	kCellMaterial_Default = 0				UMETA(DisplayName = "DEFAULT"),
	kCellMaterial_MoveHighlight = 1			UMETA(DisplayName = "MOVEHIGHLIGHT"),
	kCellMaterial_AttackHighlight = 2		UMETA(DisplayName = "ATTACKHIGHLIGHT"),
	kCellMaterial_CurrentCell = 3			UMETA(DisplayName = "CURRENTCELL")
};


UCLASS()
class TACTICS_API ACell : public AActor
{
	GENERATED_BODY()

public:

	ACell();

	void Init(int newID, AGrid* newGrid);

	// Setters
	void SetID(int newID);
	void SetGridPointer(AGrid* newGrid);
	void SetCharacterPointer(ACustomCharacter* newCharacter);
	void SetType(CellType newType);
	void SetMeshSize(float new_size);

	void SetNeighbours(TArray<ACell*> new_neighbours);
	void SetParent(ACell* parent_cell);


	// Getters
	int GetID();
	AGrid* GetGridPointer();
	CellType GetType();

	int GetRow();
	int GetColumn();

	ACustomCharacter* GetCharacterPointer();

	ACell* GetParent();
	TArray<ACell*> GetNeighbours();

	// Checkers
	bool IsWalkable();

	// Materials
	void SetCellMaterial();
	void SetCellHighlightMaterial();
	void SetCellSpawnMaterial(int spawn_team);
	void SetCellCurrentCellMaterial();

	void HighlightCell(CellMaterial);

	bool bvisited = false;				// Is this cell have been searched already?
	float globalGoal;					// Distance to Goal so far
	float localGoal;					// Distance to Goal if we took an alternative path


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, Category = "Cell")
		USceneComponent* CellRootComponent;

	UPROPERTY(EditAnywhere, Category = "Cell")
		UStaticMeshComponent* CellMeshComponent;

	UPROPERTY(EditAnywhere, Category = "Cell")
		TArray<UMaterial*> CellMeshMaterials;

	UPROPERTY(EditAnywhere, Category = "Cell")
		TArray<UMaterial*> CellHighLightMaterials;

	UPROPERTY(EditAnywhere, Category = "Cell")
		TArray<UMaterialInstance*> CellSpawnMaterials;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Cell", meta = (AllowPrivateAccess = "true"))
		AGrid* Grid;

	UPROPERTY(VisibleAnywhere, Category = "Cell")
		TEnumAsByte<CellType> type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cell", meta = (AllowPrivateAccess = "true"))
		ACustomCharacter* Character;

	// Vector to store each of its neighbours
	UPROPERTY(VisibleAnywhere, Category = "Cell")
		TArray<ACell*> neighbours;

	UPROPERTY(VisibleAnywhere, Category = "Cell")
		ACell* parent;

	int id;
	int col;
	int row;
};
