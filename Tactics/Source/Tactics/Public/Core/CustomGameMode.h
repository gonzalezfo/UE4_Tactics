// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Grid/Grid.h"

#include "CustomGameMode.generated.h"

//TODO: Put this in a Class and maybe use them to build the spawns of the Grid later.
USTRUCT(BlueprintType)
struct FTeam
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
		TArray<ACustomCharacter*> TeamMembers;

	UPROPERTY(VisibleAnywhere)
		TEnumAsByte<ESpawnTeam> TeamId;

	UPROPERTY(VisibleAnywhere)
		bool Defeated = false;
};

UCLASS()
class TACTICS_API ACustomGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

private:

	void SetGameTeamsFromGridSpawns(AGrid* grid);

	void NextTurn();
	void SetTurn(int value);

	bool CheckForNextTurn();

public: 

	//Sets default values for this actor's properties.
	ACustomGameMode();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "GameMode Variables")
		AGrid* GameGrid;

	UPROPERTY(VisibleAnywhere, Category = "GameMode Variables")
		TArray<FTeam> GameTeams;

	UPROPERTY(VisibleAnywhere, Category = "GameMode Variables")
		int CurrentTeamTurn;
	
};
