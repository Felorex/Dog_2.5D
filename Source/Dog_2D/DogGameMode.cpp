// Fill out your copyright notice in the Description page of Project Settings.


#include "DogGameMode.h"
#include "Kismet/GameplayStatics.h"



void ADogGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	SetGameStateMode(CurrentState);
}

void ADogGameMode::SetGameStateMode(EGameStateMode NewState)
{
	CurrentState = NewState;

	if (GEngine)
	{
		FString StateName = (CurrentState == EGameStateMode::MainMenu) ? TEXT("MainMenu") : TEXT("Gameplay/Pause");
		UE_LOG(LogTemp, Warning, TEXT("!!! STATE SWITCH TO: %s !!!"), *StateName);
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) 
	{
		UE_LOG(LogTemp, Error, TEXT("!!! CRITICAL: PlayerController IS NULL !!!"));
		return;
	}

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
		UE_LOG(LogTemp, Warning, TEXT("!!! MainMenuWidgetClass IS VALID, CREATING WIDGET ..."));

		MenuWidget = CreateWidget<UMainMenuWidget>(PC, MainMenuWidgetClass);
		if (MenuWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("!! WIDGET CREATED SUCCESSFULLY, ADDING TO VIEWPORT !!"));
			MenuWidget->AddToViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("!! ERROR: MainMenuWidgetClass IS EMPTY !!"));
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

void ADogGameMode::ResetLevelOnCaught()
{
	if (GetWorld())
	{
		FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());

		UGameplayStatics::OpenLevel(GetWorld(), FName(*CurrentLevelName));
	}
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