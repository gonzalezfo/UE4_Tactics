// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/Cell.h"
#include "../Public/CustomCharacter.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ACell::ACell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorComponent"));
	SetRootComponent(Floor);
}

// Called when the game starts or when spawned
void ACell::BeginPlay()
{
	Super::BeginPlay();
}

void ACell::Init(int newID, AGrid* newGrid)
{
	SetGridPointer(newGrid);

	SetID(newID);

	SetActorRelativeScale3D(Grid->GetCellSize() / GetSizeOfMesh());
}

// Called every frame
void ACell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACell::SetID(int newID)
{
	Id = newID;
}

void ACell::SetGridPointer(AGrid* newGrid)
{
	Grid = newGrid;
}

void ACell::SetCharacterPointer(ACustomCharacter* newCharacter)
{
	Character = newCharacter;
}


void ACell::SetType(CellType newType)
{
	type = newType;
}

FVector ACell::GetSizeOfMesh()
{
	return Floor->GetStaticMesh()->GetBounds().GetBox().GetSize();
}

int ACell::GetID()
{
	return Id;
}

CellType ACell::GetType()
{
	return type;
}

AGrid* ACell::GetGridPointer()
{
	return Grid;
}

ACustomCharacter* ACell::GetCharacterPointer()
{
	return Character;
}

