// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CustomPlayerController.h"
#include "CameraPawn/CameraPawn.h"


void ACustomPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("Zoom", this, &ACustomPlayerController::Zoom);

	InputComponent->BindAction("Select", IE_Pressed, this, &ACustomPlayerController::Select);
}


void ACustomPlayerController::Select()
{
	CustomCamera = Cast<ACameraPawn>(this->GetPawn());

	if (CustomCamera && bIsMyTurn)
	{
		CustomCamera->Select();
	}
}


void ACustomPlayerController::Zoom(float axis)
{
	CustomCamera = Cast<ACameraPawn>(this->GetPawn());

	if (CustomCamera && bIsMyTurn)
	{
		CustomCamera->Zoom(axis);
	}
}

bool ACustomPlayerController::IsMyTurn() {
	return bIsMyTurn;
}

void ACustomPlayerController::BeginTurn() {
	bIsMyTurn = true;
}

void ACustomPlayerController::EndTurn() {
	bIsMyTurn = false;
}


