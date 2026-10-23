// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PauseMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Core/DogGameInstance.h"

void UPauseMenuWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	if (Btn_Resume)
	{
		Btn_Resume->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	}
	if (Btn_ToMainMenu)
	{
		Btn_ToMainMenu->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnToMainMenuClicked);
	}
	if (Btn_ExitGame)
	{
		Btn_ExitGame->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnExitGameClicked);
	}
}

void UPauseMenuWidget::OnResumeClicked()
{
	OnResumePressed.Broadcast();
	/*if (GetWorld())
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);

		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			PC->bShowMouseCursor = false;
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
		}

		RemoveFromParent();
	}*/
}

void UPauseMenuWidget::OnToMainMenuClicked()
{
	if (GetWorld())
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);

		UDogGameInstance* DogGI = UDogGameInstance::Get(GetWorld());
		if (DogGI)
		{
			FName MainMenuLevel = DogGI->GetMainMenuMapName();
			UGameplayStatics::OpenLevel(GetWorld(), MainMenuLevel);
		}
	}
}
void UPauseMenuWidget::OnExitGameClicked()
{
	if (GetWorld())
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
		}
	}
}