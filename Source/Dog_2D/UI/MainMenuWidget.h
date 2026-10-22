// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class DOG_2D_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Start;
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Exit;

	UFUNCTION()
	void OnStartClicked();
	UFUNCTION()
	void OnExitClicked();
};