// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPawn/CameraPawn.h"
#include "Grid/Grid.h"
#include "Grid/Cell.h"
#include "Character/CustomCharacter.h"
#include "Widgets/CharacterHUDWidget.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "DrawDebugHelpers.h"


// Sets default values
ACameraPawn::ACameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize vars
	Margin = 3;
	CamSpeed = 1500.0f;
	SpringArmLength = 150.0f;
	ZoomModifier = 50.0f;
	ZoomOut = 1000.0f;
	ZoomIn = 150.0f;
	

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Root);
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetRelativeRotation(FRotator(-50.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = SpringArmLength;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArm);

}

// Called when the game starts or when spawned
void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();

	PC = Cast<APlayerController>(GetController());
	if(PC)
		PC->bShowMouseCursor = true;
	
	ZoomIn *= -1; // To put positive values in the Editor

	FVector2D tmp_size;
	tmp_size.X = (grid_->GridSize.X * grid_->CellSize);
	tmp_size.Y = (grid_->GridSize.X * grid_->CellSize);

	if (grid_)
	{
		ClampMin.X = -(tmp_size.X / 2.0f);
		ClampMin.Y = -(tmp_size.Y / 2.0f);
		ClampMax.X = tmp_size.Y;
		ClampMax.Y = tmp_size.X;
	}

	SetActorLocation(FVector(tmp_size.X / 2, tmp_size.Y / 2, 720.0f));
}

// Called every frame
void ACameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (PC)
		PC->GetViewportSize(ScreenSizeX, ScreenSizeY);

	PanDirection = GetCameraPanDirection() * CamSpeed * DeltaTime;
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
		newPosition.X = FMath::Clamp(newPosition.X, ClampMin.X, ClampMax.X);
		newPosition.Y = FMath::Clamp(newPosition.Y, ClampMin.Y, ClampMax.Y);

		SetActorLocation(newPosition, false, nullptr, ETeleportType::None);
	}
}

bool ACameraPawn::CheckCharacterSelected()
{
	return (character_ != nullptr);
}

bool ACameraPawn::CheckCellSelected()
{
	return (cell_ != nullptr);
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
		if (cchar->bTurnAvailable) {
			character_ = cchar;
			character_->Selected();
			character_->SetCameraPointer(this);
			PC->SetViewTargetWithBlend(character_, 1.0f);
			return;
		}
	}
	
	ACell* cell = Cast<ACell>(hitResult);

	if (cell)
	{
		cchar = cell->GetCharacterPointer();

		if (cchar) 
		{
			if (cchar->bTurnAvailable) {
				character_ = cchar;
				character_->Selected();
				character_->SetCameraPointer(this);
				PC->SetViewTargetWithBlend(character_, 1.0f);
			}
		}
	}
}

void ACameraPawn::SelectCell()
{
	AActor* hitResult = DoLineTrace();

	//If the user clicks on a character we get its cell and then return.
	ACustomCharacter* cchar = Cast<ACustomCharacter>(hitResult);

	if (cchar) {
		cell_ = cchar->GetCell();
		return;
	}

	//If it is not a character then we check if it is a cell.
	ACell* cell = Cast<ACell>(hitResult);

	if (cell)
	{
		cell_ = cell;
		return;
	}
}

void ACameraPawn::MoveCharacterToCell()
{
	if (cell_ && character_)
	{
		if (character_->GetMovableCells().Contains(cell_)) {
			AGrid* grid = cell_->GetGridPointer();

			if (grid)
			{
				character_->Unselected();
				grid->MoveCharacterToCell(character_, cell_);

				ResetCellSelection();
			}
		}
	}
}

void ACameraPawn::Attack()
{
	if (character_)
	{
		character_->Attack(cell_);
		ResetCellSelection();
		character_->bHasAttackedThisTurn = true;
	}
}

void ACameraPawn::Defend()
{
	if (character_ )
	{
		character_->Defend();
	}
}

void ACameraPawn::Heal()
{
	if (character_)
	{
		character_->Heal(cell_);
		ResetCellSelection();
	}
}

void ACameraPawn::EndTurn() 
{
	if (character_)
	{
		character_->EndTurn();
		ResetSelection();
	}
}

void ACameraPawn::ResetCellSelection()
{
	cell_ = nullptr;
}

void ACameraPawn::ResetCharacterSelection()
{
	character_ = nullptr;
}

void ACameraPawn::ResetSelection()
{
	ResetCellSelection();
	ResetCharacterSelection();
}

void ACameraPawn::Zoom(float axis)
{
	SpringArmLength -= (axis * ZoomModifier);

	if (SpringArmLength > ZoomOut)
	{
		SpringArmLength = ZoomOut;
	}
	else if (SpringArmLength < ZoomIn)
	{
		SpringArmLength = ZoomIn;
	}

	SpringArm->TargetArmLength = SpringArmLength;
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
		SelectCharacter();
	}
	else
	{
		DoCharacterAction();
	}
}

void ACameraPawn::DoCharacterAction()
{
	if (CheckCharacterSelected())
	{
		switch (character_->HUDWidget->selected_action_)
		{
		case kSelectedAction_Moving:
			SelectCell();
			if (CheckCellSelected()) 
			{
				character_->mesh_->PlayAnimation(character_->walk, true);
				MoveCharacterToCell();
			}
			break;
		case kSelectedAction_Attacking:
			SelectCell();
			if (CheckCellSelected())
			{
				Attack();
			}
			break;
		case kSelectedAction_Defending:
			if (character_)
			{
				Defend();
			}
			break;
		case kSelectedAction_Heal:
			SelectCell();
			if (CheckCellSelected())
			{
				Heal();
			}
			break;
		case kSelectedAction_EndTurn:
			if (character_)
			{
				EndTurn();
			}
			break;
		case kSelectedAction_None:
			break;
		}
	}
}

ACustomCharacter* ACameraPawn::GetCharacter()
{
	return character_;
}

