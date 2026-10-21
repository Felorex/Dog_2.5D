// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UI/MainMenuWidget.h"
#include "UI/PauseMenuWidget.h"
#include "UI/CaughtWidget.h"
#include "DogGameMode.generated.h"


UENUM(BlueprintType)
enum class EGameStateMode :uint8
{
	MainMenu,
	Gameplay,
	PauseMenu,
	Caught
};
/**
 * 
 */
UCLASS()
class DOG_2D_API ADogGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode Setting")
	EGameStateMode CurrentState;

	UFUNCTION(BlueprintCallable, Category = "GameMode Settings")
	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "Game Lifecycle")
	void OnPlayerCaught();

protected:

	virtual void PostLogin(APlayerController* NewPlayer) override;

	void SetGameStateMode(EGameStateMode NewState);

	void SetupMainMenuMode(APlayerController* PC);
	void SetupGameplayMode(APlayerController* PC);
	void SetupPauseMenuMode(APlayerController* PC);
	void SetupCaughtMode(APlayerController* PC);

	void OnCaughtSequenceFinished();
	void ResetLevelOnCaught();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menu Config")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menu Config")
	TSubclassOf<UPauseMenuWidget> PauseMenuWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menu Config")
	TSubclassOf<UCaughtWidget> CaughtWidgetClass;

	UPROPERTY()
	UMainMenuWidget* MenuWidget;

	UPROPERTY()
	UPauseMenuWidget* PauseMenuWidget;

	UPROPERTY()
	UCaughtWidget* CaughtWidget;
};
