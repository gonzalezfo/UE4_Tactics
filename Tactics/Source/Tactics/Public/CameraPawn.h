// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CameraPawn.generated.h"


class USpringArmComponent;
class UCameraComponent;
class APlayerController;


UCLASS()
class TACTICS_API ACameraPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACameraPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector GetCameraPanDirection();

	void PanMoveCamera(FVector& Direction);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:
	// References to classes and components
	USceneComponent* Root;

	USpringArmComponent* SpringArm;

	UCameraComponent* CameraComp;

	APlayerController* PC;

	float Margin;

	int32 ScreenSizeX;
	
	int32 ScreenSizeY;

	FVector PanDirection;

	UPROPERTY(EditAnywhere, Category = Cam)
		float CamSpeed;
};
