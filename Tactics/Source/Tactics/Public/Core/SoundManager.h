// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoundManager.generated.h"


class UAudioComponent;
class USoundCue;

UENUM(BlueprintType)
enum ESounds {
	kESounds_UI = 0				UMETA(DisplayName = "UI"),
	kESounds_Menu				UMETA(DisplayName = "Menu"),
	kESounds_Battle				UMETA(DisplayName = "Battle"),
	kESounds_Victory			UMETA(DisplayName = "Victory"),
	kESounds_Defeat				UMETA(DisplayName = "Defeat"),
	kESounds_Steps				UMETA(DisplayName = "Steps"),
	kESounds_Heal				UMETA(DisplayName = "Heal"),
	kESounds_Sword				UMETA(DisplayName = "Sword"),
};


UCLASS()
class TACTICS_API ASoundManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoundManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Sound
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sound")
		TArray<UAudioComponent*> AudioComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		TArray<USoundCue*> SoundCues;
};
