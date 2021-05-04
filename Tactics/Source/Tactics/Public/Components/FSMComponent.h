#pragma once

#include "CoreMinimal.h"
#include "Character/CustomCharacter.h"
#include "Components/ActorComponent.h"
#include "Grid/Grid.h"
#include "Grid/Cell.h"
#include "FSMComponent.generated.h"

UENUM(BlueprintType)
enum CharacterStates {
	//Substatees
	kCharacterState_IDLE = 0						UMETA(DisplayName = "IDLE"),
	kCharacterState_CHASE = 1						UMETA(DisplayName = "CHASE"),
	kCharacterState_RETREAT = 2						UMETA(DisplayName = "RETREAT"),
	kCharacterState_ATTACK = 3						UMETA(DisplayName = "ATTACK"),
	kCharacterState_DEFEND = 4						UMETA(DisplayName = "DEFEND"),
	kCharacterState_HEAL = 5						UMETA(DisplayName = "HEAL"),

	//States
	kCharacterState_OFFENSIVE = 10					UMETA(DisplayName = "OFFENSIVE"),
	kCharacterState_DEFFENSIVE = 11					UMETA(DisplayName = "DEFFENSIVE"),

	//Invalid
	kCharacterState_ERROR = 255						UMETA(DisplayName = "INVALID STATE"),
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TACTICS_API UFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFSMComponent();

	void BeginTurn();

	void UpdateCharacterState();

	void UpdateCharacterSubState();

	void CharacterAction();

	void SetTargetCharacter(ACustomCharacter* new_target);

	void SetTargetCell(ACell* new_target);

	void SetGrid(AGrid* new_grid);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private: 

	UPROPERTY(VisibleAnywhere, Category = "FSM Values")
		TEnumAsByte<CharacterStates> state_ = CharacterStates::kCharacterState_IDLE;

	UPROPERTY(VisibleAnywhere, Category = "FSM Values")
		TEnumAsByte<CharacterStates> sub_state_ = CharacterStates::kCharacterState_IDLE;

	UPROPERTY(VisibleAnywhere, Category="FSM Values")
		ACustomCharacter* owner_;

	UPROPERTY(VisibleAnywhere, Category = "FSM Values")
		ACustomCharacter* target_character_;

	UPROPERTY(VisibleAnywhere, Category = "FSM Values")
		ACell* target_cell_;


	//Check Used Heal (One per Character).
	UPROPERTY(VisibleAnywhere, Category = "FSM Values")
		bool bUsedHeal_;

	//Check Character Movement.
	UPROPERTY(VisibleAnywhere, Category = "FSM Values")
		bool bMoved_;

	//Check Action Conts Heal / Attack / Deffend.
	UPROPERTY(VisibleAnywhere, Category = "FSM Values")
		bool bUsedAction_;

	//Check Action Conts Heal / Attack / Deffend.
	UPROPERTY(VisibleAnywhere, Category = "FSM Values")
		bool bTargetReached_;

	AGrid* game_grid_;

	void Chase();

};
