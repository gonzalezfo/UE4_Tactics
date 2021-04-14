// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/FloatingTextWidget.h"
#include "Components/TextBlock.h"

void UFloatingTextWidget::DoFloatingText(float damage)
{
	FloatingText->SetText(FText::AsNumber(damage));
	PlayAnimation(Fade);
}
