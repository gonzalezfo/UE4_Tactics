// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CustomCharacter.generated.h"

class ACell;
class UCharacterHUDWidget;


UCLASS()
class TACTICS_API ACustomCharacter : public APawn
{
	GENERATED_BODY()

public:
	//Sets default values for this actor's properties.
	ACustomCharacter();

protected:
	//Called when the game starts or when spawned.
	virtual void BeginPlay() override;

	//Pointer to the current character cell.
	UPROPERTY(VisibleAnywhere, Category = "Cell")
		ACell* current_cell_;

public:
	//Called every frame.
	virtual void Tick(float DeltaTime) override;

	//Functions of character selection and unselection.
	void Selected();
	void Unselected();

	//Initialize the player.
	void InitPlayer(ACell*);

	//Getters
	//Gets an array of the selectable cells of the player with a certain range.
	TArray<ACell*> GetSelectableCells();

	//Gets the current character cell.
	ACell* GetCell();

	//Setters.
	void SetCell(ACell*);

	//Character static mesh.
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		UStaticMeshComponent* mesh_;

	//Character range of action.
	UPROPERTY(EditAnywhere, Category = "Properties")
		int range_;

	//Character name.
	UPROPERTY(EditAnywhere, Category = "Properties")
		FString name_;

	//Reference to the character HUD widget class.
	UPROPERTY(EditAnywhere, Category = "References")
		TSubclassOf<UCharacterHUDWidget> HUDWidgetClass;

	//Reference to the character HUD widget.
	UPROPERTY(VisibleAnywhere, Category = "References")
		UCharacterHUDWidget* HUDWidget;
};
