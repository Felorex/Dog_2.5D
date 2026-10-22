// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"


class UButton;
/**
 * 
 */
UCLASS()
class DOG_2D_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Resume;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_ToMainMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_ExitGame;

	UFUNCTION()
	void OnResumeClicked();
	UFUNCTION()
	void OnToMainMenuClicked();
	UFUNCTION()
	void OnExitGameClicked();
};
