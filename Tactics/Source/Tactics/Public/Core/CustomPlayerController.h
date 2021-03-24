// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CustomPlayerController.generated.h"


class ACameraPawn;


UCLASS()
class TACTICS_API ACustomPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	virtual void SetupInputComponent() override;
	
	ACameraPawn* CustomCamera;

	void Select();
	void Zoom(float axis);
};
