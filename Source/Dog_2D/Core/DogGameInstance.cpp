// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/DogGameInstance.h"
#include "Kismet/GameplayStatics.h"

FName UDogGameInstance::GetMainMenuMapName() const
{
	return FName(*MainMenuMapName.GetAssetName());
}
FName UDogGameInstance::GetFirstLevelMapName() const
{
	return FName(*FirstLevelMapName.GetAssetName());
}

UDogGameInstance* UDogGameInstance::Get(const UObject* WorldContextObject)
{
	return Cast<UDogGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
}