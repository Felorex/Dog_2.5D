// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcDogPawn.h"
#include "DogPawn.h"
#include "kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ANpcDogPawn::ANpcDogPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentState = EDogState::InitHome;

	PlayerFounded = false;
	IsBarkingVisual = false;

	PlayerTarget = nullptr;

	AlertTimer = 0.f;
}

void ANpcDogPawn::ReturnToHome()
{
	float CurrentLoc = GetActorLocation().X;

	if (FMath::Abs(CurrentLoc - HomeX) <= 5.f && IsGrounded)
	{
		ForceStopMovement();
		Move(0.f);
		OnLookLeftVisual();

		CurrentState = EDogState::Repose;
		OnCrouchPressed();

		//tick disable
	}
	else
	{
		float Direction = (CurrentLoc < HomeX) ? 1.f : -1.f;
		Move(Direction);
	}
}

void ANpcDogPawn::CanInteractWithObjects()
{
	Super::CanInteractWithObjects();

	if (PlayerFounded) return;

	ADogPawn* Player = Cast<ADogPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (!Player) return;	

	if (Player)
	{
		if (CurrentState == EDogState::InitHome)
		{
			return;
		}

		if (CurrentState == EDogState::Repose)
		{
			if (Player->GetActorLocation().X > TerritoryTrigger->GetActorLocation().X)
			{
				PlayerTarget = Player;
				PlayerFounded = true;
			}
		}
		else
		{
			if ((Player->GetActorLocation().X > TargetDisappeared->GetActorLocation().X))
			{
				PlayerTarget = Player;
				PlayerFounded = true;
			}
		}
	}
}
void ANpcDogPawn::StartToChase()
{
	if (!PlayerTarget) return;

	if (CheckTargetInTriggerZone())
	{
		OnCrouchReleased();
		TransitionToChase();
	}
}

float ANpcDogPawn::GetDistanceToTarget() const
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

	if (GetDistanceToTarget() <= 8.f)
	{
		return true;
	}
	return false;
}
bool ANpcDogPawn::CheckTargetInTriggerZone() const
{
	if (!TerritoryTrigger || !PlayerTarget) return false;

	return PlayerTarget->GetActorLocation().X > TerritoryTrigger->GetActorLocation().X;	
}
float ANpcDogPawn::GetStopDistance() const
{
	if (!TerritoryTrigger) return TerritoryRadius;

	float TriggerX = TerritoryTrigger->GetActorLocation().X;

	return TriggerX - TerritoryRadius;
}

void ANpcDogPawn::ChaseMovement()
{
	if (!PlayerTarget) return;

	float PlayerX = PlayerTarget->GetActorLocation().X;
	float DogX = GetActorLocation().X;

	if (IsAtLeashEdge())
	{
		ForceStopMovement();
		Move(0.f);
		return;
	}

	float Direction = (PlayerX > DogX) ? 1.f : -1.f;
	Move(Direction);
}
bool ANpcDogPawn::IsAtLeashEdge() const
{
	float DogX = GetActorLocation().X;

	return (DogX <= GetStopDistance());
}

bool ANpcDogPawn::CheckTargetVisible() const
{
	if (!TargetDisappeared || !PlayerTarget) return false;

	float DisappearedX = TargetDisappeared->GetActorLocation().X;
	float PlayerX = PlayerTarget->GetActorLocation().X;

	return (PlayerX > DisappearedX);
}

void ANpcDogPawn::StartBarking()
{
	if (IsBarkingVisual) return;

	IsBarkingVisual = true;

	OnBarkingVisual();	
}
void ANpcDogPawn::TransitionToBarking()
{
	CurrentState = EDogState::Barking;
}
void ANpcDogPawn::TransitionToChase()
{
	CurrentState = EDogState::Chase;
}
void ANpcDogPawn::TransitionToAlert()
{
	if (IsBarkingVisual)
	{
		IsBarkingVisual = false;
		OnStopBarkingVisual();
	}

	ForceStopMovement();
	Move(0.f);

	if (PlayerTarget)
	{
		float PlayerX = PlayerTarget->GetActorLocation().X;
		float CurrentLoc = GetActorLocation().X;

		if (PlayerX < CurrentLoc) { OnLookLeftVisual(); }
		else { OnLookRightVisual(); }
	}

	CurrentState = EDogState::Alert;
}
void ANpcDogPawn::StartToAlert(float DeltaTime)
{
	if (CheckTargetVisible())
	{
		if(IsAtLeashEdge())
		{
			AlertTimer = 0.f;
			TransitionToBarking();
			return;
		}
		else
		{
			AlertTimer += DeltaTime;

			if (AlertTimer > 1.f)
			{
				AlertTimer = 0;
				TransitionToChase();
			}
		}
	}
	else if (!CheckTargetVisible())
	{
		AlertTimer += DeltaTime;

		if (AlertTimer >= 3.f)
		{
			PlayerTarget = nullptr;
			PlayerFounded = false;
			AlertTimer = 0.f;

			CurrentState = EDogState::ReturnToDoghouse;
		}
	}
}

void ANpcDogPawn::Biting()
{
	if (!PlayerTarget) return;

	if (IsBarkingVisual)
	{
		IsBarkingVisual = false;
		OnStopBarkingVisual();
	}

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

	if (GEngine) {
		FString StateText = TEXT("Unknown");
		switch (CurrentState) 
		{ 
		case EDogState::Repose:
			StateText = TEXT("Repose (Sitting in Doghouse)"); 
			break; 
		case EDogState::Chase:
			StateText = TEXT("Chase (Running to Leash Edge)");
			break; 
		case EDogState::Barking:
			StateText = TEXT("Barking (At the Edge)");
			break; 
		case EDogState::Alert:
			StateText = TEXT("Alert (Watching/Bding)"); 
			break; 
		case EDogState::ReturnToDoghouse:
			StateText = TEXT("ReturnToDoghouse (Going Home)"); 
			break; 
		}
		FString FoundedText = PlayerFounded ? TEXT("TRUE (Found)") : TEXT("FALSE (Not Found)");
		FString TargetText = PlayerTarget ? FString::Printf(TEXT("VALID (%s)"), *PlayerTarget->GetName()) : TEXT("NULLPTR (Empty Memory)");
		FString VisibleText = CheckTargetVisible() ? TEXT("TRUE (Visible)") : TEXT("FALSE (Hidden)");
		FString EdgeText = IsAtLeashEdge() ? TEXT("TRUE (At Edge)") : TEXT("FALSE (Not At Edge)");
		
		GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Yellow, FString::Printf(TEXT("[Dog State]: %s"), *StateText));
		GEngine->AddOnScreenDebugMessage(2, 0.f, FColor::Cyan, FString::Printf(TEXT("[Player Founded]: %s"), *FoundedText));
		GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Orange, FString::Printf(TEXT("[Player Target]: %s"), *TargetText));
		GEngine->AddOnScreenDebugMessage(4, 0.f, FColor::Purple, FString::Printf(TEXT("[Target Visible]: %s"), *VisibleText));
		GEngine->AddOnScreenDebugMessage(5, 0.f, FColor::Green, FString::Printf(TEXT("[Is At Leash Edge]: %s"), *EdgeText));
	}


	switch (CurrentState)	
	{
	case EDogState::InitHome:
		ReturnToHome();
		break;
	case EDogState::Repose:
		StartToChase();
		break;
	case EDogState::Chase:		
		if (!CheckTargetVisible()) { TransitionToAlert(); }
		else if (IsAtLeashEdge()) { TransitionToBarking(); }
		else { ChaseMovement(); }
		break;
	case EDogState::Barking:
		StartBarking();
		if (!CheckTargetVisible()) { TransitionToAlert(); }
		break;
	case EDogState::Alert:
		StartToAlert(DeltaTime);
		break;
	case EDogState::ReturnToDoghouse:
		ReturnToHome();
		if (CheckTargetVisible()) { TransitionToAlert(); }
		break;
	default:
		break;
	}

	if (CheckBiting())
	{
		Biting();
	}
}
