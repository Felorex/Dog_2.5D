// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CaughtWidget.h"
#include "Kismet/GameplayStatics.h"

void UCaughtWidget::BroadcastAnimationFinished()
{
	OnCaughtAnimationFinished.Broadcast();
}