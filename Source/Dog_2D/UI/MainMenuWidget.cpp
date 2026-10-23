// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Core/DogGameInstance.h"


void UMainMenuWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	if (Btn_Start)
	{
		Btn_Start->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartClicked);
	}
	if (Btn_Exit)
	{
		Btn_Exit->OnClicked.AddDynamic(this, &UMainMenuWidget::OnExitClicked);
	}
}

void UMainMenuWidget::OnStartClicked()
{
	if (GetWorld())
	{
		UDogGameInstance* DogGI = UDogGameInstance::Get(GetWorld());
		if (DogGI)
		{
			FName FirstLevel = DogGI->GetFirstLevelMapName();

			UGameplayStatics::OpenLevel(GetWorld(), FirstLevel);
		}
	}
}
void UMainMenuWidget::OnExitClicked()
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
