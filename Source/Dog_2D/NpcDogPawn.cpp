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
	CanBite = false;

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

float ANpcDogPawn::GetDistance() const
{
	if (!PlayerTarget) return GetActorLocation().X;

	float PlayerX = PlayerTarget->GetActorLocation().X;
	float DogX = GetActorLocation().X;

	return (PlayerX > DogX) ? (PlayerTarget->GetMinCollisionX() - GetMaxCollisionX()) : (GetMinCollisionX() - PlayerTarget->GetMaxCollisionX());
}
bool ANpcDogPawn::CheckBiting() const
{
	if (!PlayerTarget) return false;

	if (PlayerTarget->GetIsScared())
	{
		return false;
	}

	if (GetDistance() <= 8.f)
	{
		return true;
	}
	return false;
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

	float Direction = (PlayerX > DogX) ? 1.f : -1.f;
	Move(Direction);
}
void ANpcDogPawn::Biting()
{
	if (!PlayerTarget) return;

	ForceStopMovement();
	Move(0.f);
	OnBitingVisual();
	PlayerTarget->SetIsScared(true);
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
	if (CheckBiting())
	{
		Biting();
	}

}
