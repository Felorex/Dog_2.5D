// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CaughtWidget.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCaughtAnimationFinished);

UCLASS()
class DOG_2D_API UCaughtWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCaughtAnimationFinished OnCaughtAnimationFinished;

protected:

	UFUNCTION(BlueprintCallable, Category = "Events")
	void BroadcastAnimationFinished();

};
