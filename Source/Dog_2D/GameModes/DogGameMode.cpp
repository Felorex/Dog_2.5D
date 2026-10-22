// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/DogGameMode.h"
#include "Kismet/GameplayStatics.h"



void ADogGameMode::PostLogin(APlayerController* NewPlayer)
{
	AGameModeBase::PostLogin(NewPlayer);

	SetGameStateMode(CurrentState);
}

void ADogGameMode::SetGameStateMode(EGameStateMode NewState)
{
	CurrentState = NewState;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	switch (CurrentState)
	{
	case EGameStateMode::MainMenu:
		SetupMainMenuMode(PC);
		break;
	case EGameStateMode::Gameplay:
		SetupGameplayMode(PC);
		break;
	case EGameStateMode::PauseMenu:
		SetupPauseMenuMode(PC);
		break;
	case EGameStateMode::Caught:
		SetupCaughtMode(PC);
		break;
	default:
		break;
	}
}

void ADogGameMode::SetupMainMenuMode(APlayerController* PC)
{
	DefaultPawnClass = nullptr;

	PC->bShowMouseCursor = true;
	FInputModeUIOnly InputMode;
	PC->SetInputMode(InputMode);

	if (MainMenuWidgetClass)
	{
		MenuWidget = CreateWidget<UMainMenuWidget>(PC, MainMenuWidgetClass);
		if (MenuWidget)
		{
			MenuWidget->AddToViewport();
		}
	}
}
void ADogGameMode::SetupGameplayMode(APlayerController* PC)
{
	PC->bShowMouseCursor = false;
	FInputModeGameOnly InputMode;
	PC->SetInputMode(InputMode);
}
void ADogGameMode::SetupPauseMenuMode(APlayerController* PC)
{
	if (PauseMenuWidgetClass)
	{
		PauseMenuWidget = CreateWidget<UPauseMenuWidget>(PC, PauseMenuWidgetClass);
		if (PauseMenuWidget)
		{
			PauseMenuWidget->AddToViewport();
		}

		UGameplayStatics::SetGamePaused(GetWorld(), true);
		PC->bShowMouseCursor = true;

		FInputModeUIOnly InputMode;
		PC->SetInputMode(InputMode);
	}
}

void ADogGameMode::SetupCaughtMode(APlayerController* PC) {
	if (PC)
	{
		PC->SetIgnoreMoveInput(true);

		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(nullptr);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
	}

	if (CaughtWidgetClass)
	{
		CaughtWidget = CreateWidget<UCaughtWidget>(PC, CaughtWidgetClass);
		if (CaughtWidget)
		{
			CaughtWidget->AddToViewport();

			CaughtWidget->OnCaughtAnimationFinished.AddDynamic(this, &ADogGameMode::OnCaughtSequenceFinished);
		}
	}
}
void ADogGameMode::OnCaughtSequenceFinished() {
	if (GetWorld())
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
		}

		ResetLevelOnCaught();

		SetGameStateMode(EGameStateMode::Gameplay);
	}
}
void ADogGameMode::ResetLevelOnCaught() {
	if (GetWorld()) {
		FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld()); 
		UGameplayStatics::OpenLevel(GetWorld(), FName(*CurrentLevelName));
	}
}

void ADogGameMode::OnPlayerCaught()
{
	SetGameStateMode(EGameStateMode::Caught);
}

void ADogGameMode::TogglePauseMenu()
{
	if (CurrentState == EGameStateMode::Gameplay)
	{
		SetGameStateMode(EGameStateMode::PauseMenu);
	}
	else if (CurrentState == EGameStateMode::PauseMenu)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);

		if (PauseMenuWidget)
		{
			PauseMenuWidget->RemoveFromParent();
			PauseMenuWidget = nullptr;
		}

		SetGameStateMode(EGameStateMode::Gameplay);
	}
}