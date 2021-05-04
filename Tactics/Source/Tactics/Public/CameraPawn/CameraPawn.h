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
class AGrid;
class UVictoryOrDefeatWidget;


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
	bool CheckCellSelected();

	//Selects the character that is going to act this turn.
	void SelectCharacter();

	//Selects a cell to do the action (moving, attacking, etc).
	void SelectCell();

	void MoveCharacterToCell();
	void Attack();
	void Defend();
	void Heal();
	void EndTurn();

	void ResetSelection();
	void ResetCellSelection();
	void ResetCharacterSelection();

	void Zoom(float axis);

	void DoCharacterAction();

	ACustomCharacter* GetCharacter();

	//Reference to the character VictoryOrDefeat widget class.
	UPROPERTY(EditAnywhere, Category = "Widget")
		TSubclassOf<UVictoryOrDefeatWidget> VictoryOrDefeatWidgetClass;

	//Reference to the character VictoryOrDefeat widget.
	UPROPERTY(VisibleAnywhere, Category = "Widget")
		UVictoryOrDefeatWidget* VictoryOrDefeatWidget;

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
		AGrid* grid_;

	UPROPERTY(EditAnywhere, Category = Cam)
		float Margin;

	UPROPERTY(EditAnywhere, Category = Cam)
		float CamSpeed;

	UPROPERTY(EditAnywhere, Category = Cam)
		FVector2D ClampMin;

	UPROPERTY(EditAnywhere, Category = Cam)
		FVector2D ClampMax;

	UPROPERTY(EditAnywhere, Category = Cam)
		float SpringArmLength;

	UPROPERTY(EditAnywhere, Category = Cam)
		float ZoomModifier;

	UPROPERTY(EditAnywhere, Category = Cam)
		float ZoomOut;

	UPROPERTY(EditAnywhere, Category = Cam)
		float ZoomIn;
};
