// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacter.h"
#include "../Public/Cell.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

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

void ACustomCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (PlayerInputComponent)
	{
		Super::SetupPlayerInputComponent(PlayerInputComponent);

		PlayerInputComponent->BindAction("Select", IE_Pressed, this, &ACustomCharacter::Select);
	}
}

void ACustomCharacter::Select()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PC == nullptr)
	{
		return;
	}

	// get mouse position
	float mouseX;
	float mouseY;
	PC->GetMousePosition(mouseX, mouseY);

	// get current camera location, rotation, direction
	FVector cameraLocation = PC->PlayerCameraManager->GetCameraLocation();
	FRotator cameraRotation = PC->PlayerCameraManager->GetCameraRotation();
	FVector cameraDirection = cameraRotation.Vector().GetSafeNormal();

	FVector traceStartLocation;
	FVector traceEndLocation;
	PC->DeprojectScreenPositionToWorld(mouseX, mouseY, traceStartLocation, cameraDirection);
	traceEndLocation = traceStartLocation + 1000 * cameraDirection;
	
	FHitResult hit;
	FCollisionQueryParams params;

	if (GetWorld()->LineTraceSingleByChannel(hit, traceStartLocation, traceEndLocation, ECC_Visibility, params))
	{
		DrawDebugLine(GetWorld(), traceStartLocation, traceEndLocation, FColor::Red, true);
	}
}

void ACustomCharacter::InitPlayer(ACell* tmp)
{
	current_cell_ = tmp;
}

