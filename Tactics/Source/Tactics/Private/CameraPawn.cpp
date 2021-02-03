// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"

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
	PC->bShowMouseCursor = true;
	
}

// Called every frame
void ACameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

