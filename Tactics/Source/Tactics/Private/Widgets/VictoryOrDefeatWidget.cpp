// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/VictoryOrDefeatWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "Engine/Texture2D.h"

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

void UVictoryOrDefeatWidget::InitWidget(bool victory, float players_alive, float max_players)
{
	PlayAnimation(Fade);

	if (StarImages.Num() > 0)
	{
		if (victory)
		{
			VictoryOrDefeatText->SetText(FText::FromString("Victory"));

			if ((players_alive / max_players) == 1.0f) // 3 stars
			{
				ScoreImage->SetBrushFromTexture(StarImages[3]);
			}
			else if ((players_alive / max_players) < 0.5f) // 1 star
			{
				ScoreImage->SetBrushFromTexture(StarImages[1]);
			}
			else //2 stars
			{
				ScoreImage->SetBrushFromTexture(StarImages[2]);
			}

			return;
		}

		ScoreImage->SetBrushFromTexture(StarImages[0]);
		VictoryOrDefeatText->SetText(FText::FromString("Defeat"));
	}
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