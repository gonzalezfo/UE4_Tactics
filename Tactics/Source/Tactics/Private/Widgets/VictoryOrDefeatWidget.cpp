// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/VictoryOrDefeatWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "Kismet/GameplayStatics.h"

void UVictoryOrDefeatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!NextLevelButton->OnClicked.Contains(this, "NextLevelButtonClicked"))
	{
		NextLevelButton->OnClicked.AddDynamic(this, &UVictoryOrDefeatWidget::NextLevelButtonClicked);
	}
	if (!RetryButton->OnClicked.Contains(this, "RetryButtonClicked"))
	{
		RetryButton->OnClicked.AddDynamic(this, &UVictoryOrDefeatWidget::RetryButtonClicked);
	}
	if (!ExitButton->OnClicked.Contains(this, "ExitButtonClicked"))
	{
		ExitButton->OnClicked.AddDynamic(this, &UVictoryOrDefeatWidget::ExitButtonClicked);
	}
}

void UVictoryOrDefeatWidget::InitWidget(bool victory)
{
	PlayAnimation(Fade);

	if (victory)
	{
		VictoryOrDefeatText->SetText(FText::FromString("Victory"));
		return;
	}

	VictoryOrDefeatText->SetText(FText::FromString("Defeat"));
}

void UVictoryOrDefeatWidget::NextLevelButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), "MainMap");
}

void UVictoryOrDefeatWidget::RetryButtonClicked()
{

}

void UVictoryOrDefeatWidget::ExitButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
}