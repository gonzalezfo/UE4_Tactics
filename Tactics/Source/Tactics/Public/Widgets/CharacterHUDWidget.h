// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterHUDWidget.generated.h"

class ACustomCharacter;
class UTextBlock;
class UButton;

UENUM(BlueprintType)
enum SelectedAction
{
	kSelectedAction_Moving = 0			UMETA(DisplayName = "MOVING"),
	kSelectedAction_Attacking = 1		UMETA(DisplayName = "ATTACKING"),
	kSelectedAction_Defending = 2		UMETA(DisplayName = "DEFENDING"),
	kSelectedAction_EndTurn = 3			UMETA(DisplayName = "END TURN"),
	kSelectedAction_Heal = 4			UMETA(DisplayName = "HEAL"),
	kSelectedAction_None = 5			UMETA(DisplayName = "NONE"),
};

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
		void MoveButtonClicked();

	UFUNCTION()
		void DefenseButtonClicked();

	UFUNCTION()
		void HealButtonClicked();

	UFUNCTION()
		void FinishTurnButtonClicked();

	//Sets the text to the player name.
	void SetCharacterName(FString);

	//A reference to the current selected character.
	ACustomCharacter* current_character_;

	SelectedAction selected_action_;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* CharacterName;

	UPROPERTY(meta = (BindWidget))
		UButton* AttackButton;

	UPROPERTY(meta = (BindWidget))
		UButton* MoveButton;

	UPROPERTY(meta = (BindWidget))
		UButton* DefenseButton;

	UPROPERTY(meta = (BindWidget))
		UButton* HealButton;

	UPROPERTY(meta = (BindWidget))
		UButton* FinishTurnButton;

protected:
	UFUNCTION()
		void PlayClickedButtonSound();
};
