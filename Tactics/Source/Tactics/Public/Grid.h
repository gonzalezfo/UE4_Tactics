// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.generated.h"


class ACell;

UENUM(BlueprintType)
enum CellType {
	kCellType_Void = 0		UMETA(DisplayName = "VOID"),
	kCellType_Wall = 1		UMETA(DisplayName = "WALL"),
	kCellType_Normal = 2     UMETA(DisplayName = "NORMAL"),
	kCellType_End = 3		UMETA(DisplayName = "END")
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


private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Instantiation, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> ActorToInstantiate;

	AActor* SpawnItem(UClass* ItemToSpawn, FVector& Position);
	
	void CreateGrid();

	float RoomLength;
	float RoomWidth;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Size)
		float SquareWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Size)
		int GridSizeX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Size)
		int GridSizeY;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		TArray<ACell*> Cells;
};
