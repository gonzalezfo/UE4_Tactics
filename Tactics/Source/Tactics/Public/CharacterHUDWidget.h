// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterHUDWidget.generated.h"

class ACustomCharacter;

class UTextBlock;
class UButton;

/**
 * 
 */
UCLASS()
class TACTICS_API UCharacterHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	//Button Clicked functions.
	UFUNCTION()
		void AttackButtonClicked();

	UFUNCTION()
		void DefenseButtonClicked();

	UFUNCTION()
		void FinishTurnButtonClicked();

	//Sets the text to the player name.
	void SetCharacterName(FString);

	//A reference to the current selected character.
	ACustomCharacter* current_character_;

protected:
	UPROPERTY(meta = (BindWidget))
		UTextBlock* CharacterName;

	UPROPERTY(meta = (BindWidget))
		UButton* AttackButton;

	UPROPERTY(meta = (BindWidget))
		UButton* DefenseButton;

	UPROPERTY(meta = (BindWidget))
		UButton* FinishTurnButton;
};
