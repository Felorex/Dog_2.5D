// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcDogPawn.h"


ANpcDogPawn::ANpcDogPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentState = EDogState::Alert;

	AiMoveDirection = 1.f;
}

void ANpcDogPawn::ChangeMovementDirection()
{
	AiMoveDirection *= -1.f;
}

void ANpcDogPawn::CheckHomeLocation()
{
	float CurrentLoc = GetActorLocation().X;

	if (FMath::Abs(CurrentLoc - HomeX) <= 5.f && IsGrounded)
	{
		OnLookLeftVisual();

		CurrentState = EDogState::Repose;
		OnCrouchPressed();
	}
}

void ANpcDogPawn::BeginPlay()
{
	Super::BeginPlay();

	HomeX = GetActorLocation().X;

	GetWorldTimerManager().SetTimer(
		PatrolTimerHandle,
		this,
		&ANpcDogPawn::ChangeMovementDirection,
		3.0,
		true
	);
}

void ANpcDogPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == EDogState::Alert)
	{
		CheckHomeLocation();
	}
	//Move(AiMoveDirection);
}
