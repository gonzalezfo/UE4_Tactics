// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/VictoryOrDefeatWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "Engine/Texture2D.h"

#include "Kismet/GameplayStatics.h"

#include "UObject/ConstructorHelpers.h"

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

	/*UTexture2D* Tex = LoadObject<UTexture2D>(NULL, TEXT("Texture2D'/Game/Assets/Sprites/Menu/Defeat.Defeat'"));

	static ConstructorHelpers::FClassFinder<UTexture2D> DefeatImageObj(TEXT("Texture2D'/Game/Assets/Sprites/Menu/Defeat.Defeat'"));
	StarImages.Add(Cast<UTexture2D>(DefeatImageObj.Class));

	static ConstructorHelpers::FClassFinder<UTexture2D> Star1ImageObj(TEXT("/Game/Assets/Sprites/Menu/Victory_1Star"));
	StarImages.Add(Cast<UTexture2D>(Star1ImageObj.Class));

	static ConstructorHelpers::FClassFinder<UTexture2D> Star2ImageObj(TEXT("/Game/Assets/Sprites/Menu/Victory_2Star"));
	StarImages.Add(Cast<UTexture2D>(Star2ImageObj.Class));

	static ConstructorHelpers::FClassFinder<UTexture2D> Star3ImageObj(TEXT("/Game/Assets/Sprites/Menu/Victory_3Star"));
	StarImages.Add(Cast<UTexture2D>(Star3ImageObj.Class));*/
}

void UVictoryOrDefeatWidget::InitWidget(bool victory, int players_alive)
{
	if (players_alive != 0)
	{

	}

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