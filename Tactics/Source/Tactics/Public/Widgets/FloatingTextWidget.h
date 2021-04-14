// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FloatingTextWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;


/**
 * 
 */
UCLASS()
class TACTICS_API UFloatingTextWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
		UTextBlock* FloatingText;

	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* Fade;

	UFUNCTION()
		void DoFloatingText(float damage);
};
