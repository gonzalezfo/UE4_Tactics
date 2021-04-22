// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/FloatingTextWidget.h"

#include "Character/CustomCharacter.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/HealthComponent.h"

#include "Kismet/GameplayStatics.h"

//#include "Styling/SlateColor.h"
//#include "Math/Color.h"

void UFloatingTextWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UFloatingTextWidget::InitOverheadHUDWidget(ACustomCharacter* ref)
{
	characterReference = ref;

	if (characterReference)
	{
		if (!characterReference->HealthComp->OnHealthChanged.Contains(this, "UpdateHealthBar"))
		{
			characterReference->HealthComp->OnHealthChanged.AddDynamic(this, &UFloatingTextWidget::UpdateHealthBar);
		}
	}
}

void UFloatingTextWidget::UpdateHealthBar(UHealthComponent* OwningHealthComp, float CurrentHealth, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (characterReference)
	{
		float current_health = characterReference->HealthComp->GetCurrentHealthPercentage();
		HealthBar->SetPercent(current_health);
	}
}

void UFloatingTextWidget::DoFloatingText(float damage)
{
	if (damage < 0.0f)
	{
		//It gives a link error when I try to do this.
		//FloatingText->SetColorAndOpacity(FSlateColor(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)));
		FloatingText->SetText(FText::AsNumber(damage * -1.0f));
	}
	else
	{
		FloatingText->SetText(FText::AsNumber(damage));
	}
	
	PlayAnimation(Fade);
}
