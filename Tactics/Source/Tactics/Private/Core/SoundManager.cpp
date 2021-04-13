// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SoundManager.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
ASoundManager::ASoundManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UISoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("UISound"));
	UISoundComponent->SetupAttachment(RootComponent);
	UISoundComponent->bAutoActivate = false;
	UISoundComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}

// Called when the game starts or when spawned
void ASoundManager::BeginPlay()
{
	Super::BeginPlay();

	if (UISoundComponent != nullptr && UISoundCue->IsValidLowLevelFast()) {
		UISoundComponent->SetSound(UISoundCue);
	}
}

// Called every frame
void ASoundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

