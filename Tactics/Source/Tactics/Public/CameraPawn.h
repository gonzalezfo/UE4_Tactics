// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CameraPawn.generated.h"


class USpringArmComponent;
class UCameraComponent;
class APlayerController;
class ACustomCharacter;
class ACell;


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

	AActor* DoLineTrace();

	//INPUT
	void Select();

	bool CheckCharacterSelected();
	void SelectCharacter();
	void SelectCell();
	void MoveCharacterToCell();
	void ResetSelection();
	void Zoom(float axis);

protected:
	// References to classes and components
	USceneComponent* Root;

	USpringArmComponent* SpringArm;

	UCameraComponent* CameraComp;

	APlayerController* PC;

	ACustomCharacter* character_;
	ACell* cell_;

	int32 ScreenSizeX;
	
	int32 ScreenSizeY;

	FVector PanDirection;


	UPROPERTY(EditAnywhere, Category = Cam)
		float Margin;

	UPROPERTY(EditAnywhere, Category = Cam)
		float CamSpeed;

	UPROPERTY(EditAnywhere, Category = Cam)
		float ClampMin;

	UPROPERTY(EditAnywhere, Category = Cam)
		float ClampMax;

	UPROPERTY(EditAnywhere, Category = Cam)
		float SpringArmLength;

	UPROPERTY(EditAnywhere, Category = Cam)
		float ZoomModifier;

	UPROPERTY(EditAnywhere, Category = Cam)
		float ZoomOut;

	UPROPERTY(EditAnywhere, Category = Cam)
		float ZoomIn;
};
