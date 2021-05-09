// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SoundManager.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
ASoundManager::ASoundManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UAudioComponent* UISoundComp;
	UISoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("UI Audio Component"));

	AudioComponents.Add(UISoundComp);

	UAudioComponent* MenuSoundComp;
	MenuSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Menu Audio Component"));

	AudioComponents.Add(MenuSoundComp);

	UAudioComponent* BattleSoundComp;
	BattleSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Battle Audio Component"));

	AudioComponents.Add(BattleSoundComp);

	UAudioComponent* VictorySoundComp;
	VictorySoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Victory Audio Component"));

	AudioComponents.Add(VictorySoundComp);

	UAudioComponent* DefeatSoundComp;
	DefeatSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Defeat Audio Component"));

	AudioComponents.Add(DefeatSoundComp);

	UAudioComponent* StepsSoundComp;
	StepsSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Steps Audio Component"));

	AudioComponents.Add(StepsSoundComp);

	UAudioComponent* HealSoundComp;
	HealSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Heal Audio Component"));

	AudioComponents.Add(HealSoundComp);

	UAudioComponent* SwordSoundComp;
	SwordSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Sword Audio Component"));

	AudioComponents.Add(SwordSoundComp);

	for (int i = 0; i < AudioComponents.Num(); ++i)
	{
		AudioComponents[i]->SetupAttachment(RootComponent);
		AudioComponents[i]->bAutoActivate = false;
		AudioComponents[i]->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	}
}

// Called when the game starts or when spawned
void ASoundManager::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < AudioComponents.Num(); ++i)
	{
		if (AudioComponents[i] != nullptr && SoundCues[i]->IsValidLowLevelFast()) {
			AudioComponents[i]->SetSound(SoundCues[i]);
		}
	}	
}

// Called every frame
void ASoundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

