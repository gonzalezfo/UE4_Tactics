#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "Character/CustomCharacter.h"
#include "Grid/Grid.h"

#include "CustomAIController.generated.h"


UCLASS()
class TACTICS_API ACustomAIController : public AAIController
{
	GENERATED_BODY()

public: 

	UPROPERTY(VisibleAnywhere)
		TArray<ACustomCharacter*> AITeamCharacters;

	UPROPERTY(VisibleAnywhere)
		TArray<ACustomCharacter*> OtherTeamsCharacters;

	UPROPERTY(VisibleAnywhere)
		ACameraPawn* CameraPawn;

	UPROPERTY(VisibleAnywhere)
		AGrid* Grid;

	void BeginTurn();

	bool IsMyTurn();

protected:

	void SetCharacterRandomMovement();

	void SetNextCharacterForAction();

	void EndTurn();


private: 

	ACustomCharacter* SelectedCharacter;

	FTimerHandle TimerHandle;

	bool bIsMyTurn;

	void SetCharacterTarget();

};
