// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CustomCharacter.generated.h"

class ACell;
class ACameraPawn;
class UCharacterHUDWidget;
class UCameraComponent;
class UHealthComponent;
<<<<<<< Updated upstream
=======
class UWidgetComponent;
class USkeletalComponent;
class UAnimationAsset;
>>>>>>> Stashed changes


UENUM(BlueprintType)
enum CharacterState
{
	kCharacterState_Idle = 0				UMETA(DisplayName = "IDLE"),
	kCharacterState_Moving = 1				UMETA(DisplayName = "MOVING"),
	kCharacterState_FinishMovement = 2		UMETA(DisplayName = "FINISH MOVEMENT"),
};

UCLASS()
class TACTICS_API ACustomCharacter : public APawn
{
	GENERATED_BODY()

public:
	//Sets default values for this actor's properties.
	ACustomCharacter();

	//Checks if the given actor is friendly or not.
	UFUNCTION(BlueprintCallable, Category = "Teams")
		bool IsFriendly(AActor* other);

protected:
	//Called when the game starts or when spawned.
	virtual void BeginPlay() override;

	void ReturnToMainCamera();

	//Pointer to the current character cell.
	UPROPERTY(VisibleAnywhere, Category = "Cell")
		ACell* current_cell_;

	//Reference to the Camera Pawn.
	UPROPERTY(VisibleAnywhere, Category = "References")
		ACameraPawn* camera_pawn_;
	
	//Camera component.
	UPROPERTY(EditAnywhere, Category = "Camera Properties")
		UCameraComponent* CameraComp;

	//Health component.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health Properties")
		UHealthComponent* HealthComp;

	//Widget component.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget Properties")
		UWidgetComponent* WidgetComp;

	//Movement time
	float movement_time_;

	FTimerHandle handle_;

public:
	//Called every frame.
	virtual void Tick(float DeltaTime) override;

	//Initialize the player.
	void InitPlayer(ACell*);

	//Functions of character selection and unselection.
	void Selected();
	void Unselected();

	//Path
	TArray<ACell*> path_cells_;

	void MoveAlongPath(float DeltaTime);

	void Defend();
	void EndTurn();

	//Getters
	//Gets an array of the selectable cells of the player with a certain range.
	TArray<ACell*> GetMovableCells();

	TArray<ACell*> GetAttackCells();

	//Gets the current character cell.
	ACell* GetCell();

	int GetCharacterTeam();

	//Setters.
	//Sets the current character cell.
	void SetCell(ACell*);

	void SetCameraPointer(ACameraPawn*);

	void SetCharacterTeam(int team_value);

	void UpdateMaterial();

	void EndCharacterTurn();

	//Character static mesh.
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		UStaticMeshComponent* mesh_;


	//Character name.
	UPROPERTY(EditAnywhere, Category = "Gameplay Properties")
		FString name_;

	//Character range of action.
	UPROPERTY(EditAnywhere, Category = "Gameplay Properties")
		int range_;

	//Character movement speed.
	UPROPERTY(EditAnywhere, Category = "Gameplay Properties")
		float movement_speed_;

	//If the character is moving or not.
	UPROPERTY(EditAnywhere, Category = "Gameplay Properties")
		TEnumAsByte<CharacterState> state_;


	//Reference to the character HUD widget class.
	UPROPERTY(EditAnywhere, Category = "References")
		TSubclassOf<UCharacterHUDWidget> HUDWidgetClass;

	//Reference to the character HUD widget.
	UPROPERTY(VisibleAnywhere, Category = "References")
		UCharacterHUDWidget* HUDWidget;


	//If the character has the turn available or not.
	UPROPERTY(VisibleAnywhere, Category = "Team")
		bool TurnAvailable;

	//If the character is dead or not.
	UPROPERTY(BlueprintReadOnly, Category = "Health Properties")
		bool bDied;

	//If the character is defending itself or not.
	UPROPERTY(VisibleAnywhere, Category = "Actions")
		bool isDefending;

	//Event for the character health changing.
	UFUNCTION()
		void OnHealthChanged(UHealthComponent* OwningHealthComp, float CurrentHealth, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	TSubclassOf<UDamageType> MeleeDamage;

	UFUNCTION()
		void StartTurn();

	int cells_moved_this_turn_;

private:
	//The material of the character.
	UPROPERTY(EditAnywhere, Category = "Team")
		TArray<UMaterialInstance*> CharacterMaterials;

	//The character team.
	UPROPERTY(VisibleAnywhere, Category = "Team")
		uint8 TeamNum;

};
