// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Grid.h"

#include "CustomGameMode.generated.h"

USTRUCT(BlueprintType)
struct FTeam
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
		TArray<ACustomCharacter*> TeamMembers;

	UPROPERTY(VisibleAnywhere)
		TEnumAsByte<ESpawnTeam> TeamNumber;

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

public: 

	UPROPERTY(VisibleAnywhere, Category = "GameMode Variables")
		AGrid* GameGrid;

	UPROPERTY(VisibleAnywhere, Category = "GameMode Variables")
		TArray<FTeam> GameTeams;

	UPROPERTY(VisibleAnywhere, Category = "GameMode Variables")
		int CurrentTeamTurn;
	
};
