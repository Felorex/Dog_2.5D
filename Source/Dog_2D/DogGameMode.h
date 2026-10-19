// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DogGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DOG_2D_API ADogGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Game Lifecycle")
	void ResetLevelOnCaught();
};
