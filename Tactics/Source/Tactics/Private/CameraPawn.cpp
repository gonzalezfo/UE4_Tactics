// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "CustomCharacter.h"
#include "Cell.h"
#include "DrawDebugHelpers.h"


// Sets default values
ACameraPawn::ACameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Root);
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetRelativeRotation(FRotator(-50.0f, 0.0f, 0.0f));

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArm);

	// Initialize vars
	Margin = 3;
	CamSpeed = 15.0f;
	ClampMin = -500.0f;
	ClampMax = 500.0f;
}

// Called when the game starts or when spawned
void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();

	PC = Cast<APlayerController>(GetController());
	if(PC)
		PC->bShowMouseCursor = true;
	
}

// Called every frame
void ACameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (PC)
		PC->GetViewportSize(ScreenSizeX, ScreenSizeY);

	PanDirection = GetCameraPanDirection() * CamSpeed;
	PanMoveCamera(PanDirection);
}

// Called to bind functionality to input
void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FVector ACameraPawn::GetCameraPanDirection()
{
	float MousePosX;
	float MousePosY;
	float CamDirectionX = 0;
	float CamDirectionY = 0;

	PC->GetMousePosition(MousePosX, MousePosY);

	if (MousePosX <= Margin)
	{
		CamDirectionY = -1;
	}

	if (MousePosY <= Margin)
	{
		CamDirectionX = 1;
	}

	if (MousePosX >= ScreenSizeX - Margin)
	{
		CamDirectionY = 1;
	}

	if (MousePosY >= ScreenSizeY - Margin)
	{
		CamDirectionX = -1;
	}

	return FVector(CamDirectionX, CamDirectionY, 0.0f);
}

void ACameraPawn::PanMoveCamera(FVector& Direction)
{
	if (PanDirection == FVector::ZeroVector)
	{
		return;
	}
	else {
		AddActorWorldOffset(Direction);
		FVector newPosition = GetActorLocation();
		newPosition.X = FMath::Clamp(newPosition.X, ClampMin, ClampMax);
		newPosition.Y = FMath::Clamp(newPosition.Y, ClampMin, ClampMax);

		SetActorLocation(newPosition, false, nullptr, ETeleportType::None);
	}
}

bool ACameraPawn::CheckCharacterSelected()
{
	return (character_ != nullptr);
}

AActor* ACameraPawn::DoLineTrace()
{
	// get mouse position
	float mouseX;
	float mouseY;
	PC->GetMousePosition(mouseX, mouseY);

	// get current camera location, rotation, direction
	FRotator cameraRotation = PC->PlayerCameraManager->GetCameraRotation();
	FVector cameraDirection = cameraRotation.Vector().GetSafeNormal();

	FVector traceStartLocation;
	FVector traceEndLocation;
	PC->DeprojectScreenPositionToWorld(mouseX, mouseY, traceStartLocation, cameraDirection);
	traceEndLocation = traceStartLocation + 10000 * cameraDirection;

	FHitResult hit;
	FCollisionQueryParams params;

	if (GetWorld()->LineTraceSingleByChannel(hit, traceStartLocation, traceEndLocation, ECC_Visibility, params))
	{
		return hit.GetActor();
	}

	return nullptr;
}

void ACameraPawn::SelectCharacter()
{
	AActor* hitResult = DoLineTrace();

	ACustomCharacter* cchar = Cast<ACustomCharacter>(hitResult);

	if (cchar)
	{
		character_ = cchar;
		return;
	}
	
	ACell* cell = Cast<ACell>(hitResult);

	if (cell)
	{
		cchar = cell->GetCharacterPointer();

		if (cchar)
		{
			character_ = cchar;
		}
	}
}

void ACameraPawn::SelectCell()
{
	ACell* cell = Cast<ACell>(DoLineTrace());

	if (cell)
	{
		cell_ = cell;
	}
}

void ACameraPawn::MoveCharacterToCell()
{
	if (cell_)
	{
		AGrid* grid = cell_->GetGridPointer();

		if (grid)
		{
			if (character_)
			{
				UE_LOG(LogTemp, Warning, TEXT("You are moving the character to the new cell"));
				grid->MoveCharacterToCell(character_, cell_);
			}	
		}
	}
}

void ACameraPawn::ResetSelection()
{
	character_ = nullptr;
	cell_ = nullptr;
}

void ACameraPawn::Select()
{
	//Implement a solution to the user selecting the player current cell.
	if (PC == nullptr)
	{
		return;
	}

	if (!CheckCharacterSelected())
	{
		UE_LOG(LogTemp, Warning, TEXT("You are selecting the character"));
		SelectCharacter();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("You are selecting the cell"));
		SelectCell();
		MoveCharacterToCell();
		ResetSelection();
	}
}


