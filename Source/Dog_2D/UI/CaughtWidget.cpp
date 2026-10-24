// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CaughtWidget.h"

void UCaughtWidget::BroadcastAnimationFinished()
{
	OnCaughtAnimationFinished.Broadcast();
}
