// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VictoryOrDefeatWidget.generated.h"

class UTextBlock;
class UButton;
class UWidgetAnimation;
class UImage;
class UTexture2D;


/**
 * 
 */
UCLASS()
class TACTICS_API UVictoryOrDefeatWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* VictoryOrDefeatText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* NextLevelButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* RetryButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UButton* ExitButton;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		UImage* ScoreImage;

	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* Fade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UTexture2D*> StarImages;

public:
	UFUNCTION()
		void InitWidget(bool victory, float players_alive, float max_players);

	UFUNCTION()
		void NextLevelButtonClicked();

	UFUNCTION()
		void RetryButtonClicked();

	UFUNCTION()
		void ExitButtonClicked();
};
