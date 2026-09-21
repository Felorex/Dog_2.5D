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



void ANpcDogPawn::CanInteractWithObjects()
{
	Super::CanInteractWithObjects();

	if (PlayerFounded) return;

	else if (!PlayerFounded && CurrentState == EDogState::Repose && TerritoryTriger)
	{
		ADogPawn* Player = Cast<ADogPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

		if (Player)
		{
			PlayerTarget = Player;

			if(PlayerTarget->GetActorLocation().X > TerritoryTriger->GetActorLocation().X)
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
		float TrigerX = TerritoryTriger->GetActorLocation().X;

		if (PlayerX >= TrigerX)
		{
			CurrentState = EDogState::Chase;
			OnCrouchReleased();
		}
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

}
