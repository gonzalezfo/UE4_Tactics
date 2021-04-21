// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FloatingTextWidget.generated.h"

class ACustomCharacter;
class UTextBlock;
class UWidgetAnimation;
class UProgressBar;


/**
 * 
 */
UCLASS(Blueprintable)
class TACTICS_API UFloatingTextWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	ACustomCharacter* characterReference;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* FloatingText;

	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* Fade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UProgressBar* HealthBar;

	UFUNCTION()
		void InitOverheadHUDWidget(ACustomCharacter* ref);

	UFUNCTION()
		void UpdateHealthBar(UHealthComponent* OwningHealthComp, float CurrentHealth, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
		void DoFloatingText(float damage);
};
