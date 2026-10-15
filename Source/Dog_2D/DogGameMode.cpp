// Fill out your copyright notice in the Description page of Project Settings.


#include "DogGameMode.h"
#include "Kismet/GameplayStatics.h"

void ADogGameMode::ResetLevelOnCaught()
{
	if (GetWorld())
	{
		FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());

		UGameplayStatics::OpenLevel(GetWorld(), FName(*CurrentLevelName));
	}
}