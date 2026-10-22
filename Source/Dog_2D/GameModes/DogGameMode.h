// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DogGameMode.generated.h"

class UMainMenuWidget;
class UPauseMenuWidget;
class UCaughtWidget;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode|State")
	EGameStateMode CurrentState;

	UFUNCTION(BlueprintCallable, Category = "GameMode|Logic")
	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "GameMode|Logic")
	void OnPlayerCaught();

protected:

	virtual void PostLogin(APlayerController* NewPlayer) override;

	void SetGameStateMode(EGameStateMode NewState);

	void SetupMainMenuMode(APlayerController* PC);
	void SetupGameplayMode(APlayerController* PC);
	void SetupPauseMenuMode(APlayerController* PC);
	void SetupCaughtMode(APlayerController* PC);

	void ResetLevelOnCaught();

	UFUNCTION()
	void OnCaughtSequenceFinished();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameMode|UI Classes")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameMode|UI Classes")
	TSubclassOf<UPauseMenuWidget> PauseMenuWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameMode|UI Classes")
	TSubclassOf<UCaughtWidget> CaughtWidgetClass;

	UPROPERTY(Transient)
	UMainMenuWidget* MenuWidget;

	UPROPERTY(Transient)
	UPauseMenuWidget* PauseMenuWidget;

	UPROPERTY(Transient)
	UCaughtWidget* CaughtWidget;
};
