// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "..\Public\HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UHealthComponent::GetCurrentHealth()
{
	return currentHealth_;
}

float UHealthComponent::GetMaxHealth()
{
	return maxHealth_;
}

void UHealthComponent::TakeDamage(float damageToTake)
{
	//only take damage if its positive
	if (damageToTake > 0.0f) {
		currentHealth_ -= damageToTake;


		//if health goes below 0, cap it
		if (currentHealth_ < 0.0f) {
			currentHealth_ = 0.0f;
		}
	}
}

void UHealthComponent::Heal(float healing)
{
	//only heal if its positive
	if (healing > 0.0f) {
		currentHealth_ += healing;

		//if current health is higher than max, then cap it
		if (currentHealth_ > maxHealth_) {
			currentHealth_ = maxHealth_;
		}
	}
}

