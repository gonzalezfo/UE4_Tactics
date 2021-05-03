// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VictoryOrDefeatWidget.generated.h"

class UTextBlock;
class UButton;
class UWidgetAnimation;


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

	UPROPERTY(meta = (BindWidgetAnim))
		UWidgetAnimation* Fade;

public:
	UFUNCTION()
		void InitWidget(bool victory);

	UFUNCTION()
		void NextLevelButtonClicked();

	UFUNCTION()
		void RetryButtonClicked();

	UFUNCTION()
		void ExitButtonClicked();
};
