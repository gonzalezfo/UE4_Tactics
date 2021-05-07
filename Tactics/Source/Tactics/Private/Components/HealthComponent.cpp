// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"
#include "Character/CustomCharacter.h"
#include "Components/WidgetComponent.h"
#include "Widgets/FloatingTextWidget.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
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
	
}

void UHealthComponent::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (bIsDead)
	{
		return;
	}

	if (DamageCauser == DamagedActor && Damage > 0.0f)
	{
		return;
	}

	ACustomCharacter* tmp = Cast<ACustomCharacter>(DamagedActor);

	float damageToTake = Damage;

	if(tmp)
	{
		if (tmp->isDefending)
		{
			damageToTake *= 0.5f;
		}
	}

	////Floating Text
	UFloatingTextWidget* FloatingText = Cast<UFloatingTextWidget>(tmp->WidgetComp->GetUserWidgetObject());

	if (FloatingText)
	{
		FloatingText->DoFloatingText(damageToTake);
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - damageToTake, 0.0f, MaxHealth);

	bIsDead = CurrentHealth <= 0.0f;

	OnHealthChanged.Broadcast(this, CurrentHealth, damageToTake, DamageType, InstigatedBy, DamageCauser);
}

float UHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

float UHealthComponent::GetCurrentHealthPercentage()
{
	return CurrentHealth / MaxHealth;
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




