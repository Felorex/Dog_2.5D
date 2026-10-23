// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DogGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DOG_2D_API UDogGameInstance : public UGameInstance
{
	GENERATED_BODY()
	

public:
	FName GetMainMenuMapName() const;
	FName GetFirstLevelMapName() const;

	UFUNCTION(BlueprintPure, Category = "Game", meta = (DisplayName = "Get Dog Game Instance", WorldContext = "WorldContextObject"))
	static UDogGameInstance* Get(const UObject* WorldContextObject);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Maps", meta = (AllowedClasses = "/Script/Engine.World"))
	FSoftObjectPath MainMenuMapName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Maps", meta = (AllowedClasses = "/Script/Engine.World"))
	FSoftObjectPath FirstLevelMapName;
};
