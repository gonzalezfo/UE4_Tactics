// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralRoom.generated.h"

UCLASS()
class TACTICS_API AProceduralRoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralRoom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Room, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Floor;

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
};
