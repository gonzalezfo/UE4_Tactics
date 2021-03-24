// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerController.h"
#include "CameraPawn.h"


void ACustomPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("Zoom", this, &ACustomPlayerController::Zoom);

	InputComponent->BindAction("Select", IE_Pressed, this, &ACustomPlayerController::Select);
}


void ACustomPlayerController::Select()
{
	CustomCamera = Cast<ACameraPawn>(this->GetPawn());

	if (CustomCamera)
	{
		CustomCamera->Select();
	}
}


void ACustomPlayerController::Zoom(float axis)
{
	CustomCamera = Cast<ACameraPawn>(this->GetPawn());

	if (CustomCamera)
	{
		CustomCamera->Zoom(axis);
	}
}
