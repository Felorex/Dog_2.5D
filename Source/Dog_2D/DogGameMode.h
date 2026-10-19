// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuWidget.h"
#include "PauseMenuWidget.h"
#include "DogGameMode.generated.h"


UENUM(BlueprintType)
enum class EGameStateMode :uint8
{
	MainMenu,
	Gameplay,
	PauseMenu
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
	void ResetLevelOnCaught();

protected:

	virtual void PostLogin(APlayerController* NewPlayer) override;

	void SetGameStateMode(EGameStateMode NewState);

	void SetupMainMenuMode(APlayerController* PC);
	void SetupGameplayMode(APlayerController* PC);
	void SetupPauseMenuMode(APlayerController* PC);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menu Config")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menu Config")
	TSubclassOf<UPauseMenuWidget> PauseMenuWidgetClass;

	UPROPERTY()
	UMainMenuWidget* MenuWidget;

	UPROPERTY()
	UPauseMenuWidget* PauseMenuWidget;
};
