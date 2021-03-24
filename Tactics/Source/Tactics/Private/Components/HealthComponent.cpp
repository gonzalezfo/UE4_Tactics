// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "..\Public\HealthComponent.h"
#include "CustomCharacter.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	MaxHealth = 100.0f;
	bIsDead = false;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
	}
	
	CurrentHealth = MaxHealth;
}

void UHealthComponent::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	if (DamageCauser != DamagedActor)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);

	bIsDead = CurrentHealth <= 0.0f;

	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
}

float UHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

void UHealthComponent::Regenerate(float RegAmount)
{
	if (RegAmount <= 0.0f || CurrentHealth <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth + RegAmount, 0.0f, MaxHealth);

	OnHealthChanged.Broadcast(this, CurrentHealth, -RegAmount, nullptr, nullptr, nullptr);
}




