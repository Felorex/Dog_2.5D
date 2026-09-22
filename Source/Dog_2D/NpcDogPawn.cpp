// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcDogPawn.h"
#include "DogPawn.h"
#include "kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ANpcDogPawn::ANpcDogPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentState = EDogState::Alert;

	PlayerFounded = false;

	PlayerTarget = nullptr;

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



void ANpcDogPawn::CanInteractWithObjects()
{
	Super::CanInteractWithObjects();

	if (PlayerFounded) return;

	else if (!PlayerFounded && CurrentState == EDogState::Repose && TerritoryTrigger)
	{
		ADogPawn* Player = Cast<ADogPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

		if (Player)
		{
			PlayerTarget = Player;

			if(PlayerTarget->GetActorLocation().X > TerritoryTrigger->GetActorLocation().X)
			{
				PlayerFounded = true;

				StartToChase();
			}
		}
	}
}
void ANpcDogPawn::StartToChase()
{
	if (!PlayerTarget) return;

	if (PlayerTarget)
	{
		float PlayerX = PlayerTarget->GetActorLocation().X;
		float TrigerX = TerritoryTrigger->GetActorLocation().X;

		if (PlayerX >= TrigerX)
		{
			CurrentState = EDogState::Chase;
			OnCrouchReleased();
		}
	}
}
void ANpcDogPawn::ChaseMovement()
{
	if (!PlayerTarget) return;

	float PlayerX = PlayerTarget->GetActorLocation().X;
	float DogX = GetActorLocation().X;
	float TriggerX = TerritoryTrigger->GetActorLocation().X;

	float StopDistance = TriggerX - TerritoryRadius;

	if (DogX <= StopDistance)
	{
		ForceStopMovement();
		Move(0.f);

		return;
	}

	float Distance = (PlayerX > DogX) ? (PlayerTarget->GetMinCollisionX() - GetMaxCollisionX()) : (GetMinCollisionX() - PlayerTarget->GetMaxCollisionX());

	if (Distance > 8.f)
	{
		float Direction = (PlayerX > DogX) ? 1.f : -1.f;
		Move(Direction);
	}
	else
	{
		ForceStopMovement();
		Move(0.f);
		PlayerTarget->SetIsScared(true);
	}
}

void ANpcDogPawn::BeginPlay()
{
	Super::BeginPlay();

	HomeX = GetActorLocation().X;

}

void ANpcDogPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == EDogState::Alert)
	{
		CheckHomeLocation();
	}
	if (CurrentState == EDogState::Chase)
	{
		ChaseMovement();
	}

}
