// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.generated.h"


class ACell;


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
	float GetCellWidth();

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Instantiation, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> ActorToInstantiate;

	void SpawnItem(UClass* ItemToSpawn, FVector& Position);

	void CreateGrid();

	FVector GetMidPointInGrid(const FVector& UpperLeft, const FVector& LowerRight);

	void PlacePointsOnGrid();

	FVector TopLeft;
	FVector BottomRight;
	float RoomLength;
	float RoomWidth;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Size)
		float SquareWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Size)
		int GridSizeX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Size)
		int GridSizeY;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Size)
		float GridHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grid)
		TArray<ACell*> Cells;
};
