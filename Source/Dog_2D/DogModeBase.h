// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DogModeBase.generated.h"

/**
 * 
 */
UCLASS()
class DOG_2D_API ADogModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void RestartCurrentLevel();

};
