// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerController.h"
#include "CameraPawn.h"


void ACustomPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
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
