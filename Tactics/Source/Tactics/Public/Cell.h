// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cell.generated.h"


class AGrid;


UCLASS()
class TACTICS_API ACell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = References, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AGrid> Grid;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Room, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* Floor;

};
