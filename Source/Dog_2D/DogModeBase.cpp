// Fill out your copyright notice in the Description page of Project Settings.


#include "DogModeBase.h"
#include "Kismet/GameplayStatics.h"

void ADogModeBase::RestartCurrentLevel()
{
	if (GetWorld())
	{
		FString CurrentLevelName = GetWorld()->GetName();
		CurrentLevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

		UGameplayStatics::OpenLevel(GetWorld(), FName(*CurrentLevelName));
	}
}
