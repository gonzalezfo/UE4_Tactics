// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Public/Grid.h"
#include "Cell.generated.h"

class ACustomCharacter;


UENUM(BlueprintType)
enum CellType {
	kCellType_Void = 0		UMETA(DisplayName = "VOID"),
	kCellType_Wall = 1		UMETA(DisplayName = "WALL"),
	kCellType_Normal = 2    UMETA(DisplayName = "NORMAL"),
	kCellType_End = 3		UMETA(DisplayName = "END")
};


UCLASS()
class TACTICS_API ACell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = References, meta = (AllowPrivateAccess = "true"))
		AGrid* Grid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = References, meta = (AllowPrivateAccess = "true"))
		ACustomCharacter* Character;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Init(int newID, AGrid* newGrid);

	// Setters
	void SetID(int newID);
	void SetGridPointer(AGrid* newGrid);
	void SetCharacterPointer(ACustomCharacter* newCharacter);
	void SetType(CellType newType);


	// Getters
	FVector GetSizeOfMesh();
	int GetID();
	CellType GetType();
	AGrid* GetGridPointer();
	ACustomCharacter* GetCharacterPointer();


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Room, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* Floor;

	int Id;

	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<CellType> type;

};
