// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacter.h"
#include "../Public/Cell.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
ACustomCharacter::ACustomCharacter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Character Mesh Component"));
	SetRootComponent(mesh_);
}

// Called when the game starts or when spawned
void ACustomCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACustomCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}




void ACustomCharacter::InitPlayer(ACell* tmp)
{
	current_cell_ = tmp;
}

