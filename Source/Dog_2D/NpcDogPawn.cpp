// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcDogPawn.h"
#include "DogPawn.h"
#include "kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ANpcDogPawn::ANpcDogPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentState = EDogState::InitHome;

	bCanTakeItem = true;

	PlayerFounded = false;
	IsBarkingVisual = false;

	PlayerTarget = nullptr;

	AlertTimer = 0.f;
}

bool ANpcDogPawn::CheckHomeLocation() const
{
	float CurrentLoc = GetActorLocation().X;

	if (FMath::Abs(CurrentLoc - HomeX) <= 5.f && IsGrounded)
	{
		return true;
	}
	return false;
}
void ANpcDogPawn::ReturnToHome()
{
	float CurrentLoc = GetActorLocation().X;

	float Direction = (CurrentLoc < HomeX) ? 1.f : -1.f;
	Move(Direction);
}
void ANpcDogPawn::TransitionToRepose()
{
	ForceStopMovement();
	Move(0.f);
	OnLookLeftVisual();

	CurrentState = EDogState::Repose;
	OnCrouchPressed();
	
	//tick disable
	SetActorTickEnabled(false);
}
void ANpcDogPawn::TransitionToDeactivated()
{
	ForceStopMovement();
	Move(0.f);
	OnLookLeftVisual();

	OnCrouchPressed();

	//tick disable
	SetActorTickEnabled(false);
}
void ANpcDogPawn::CheckBoneInsideTerritory()
{
	if (IsAtLeashEdge() || !Bone) return;

	float Direction = FMath::Sign(Conteiner->GetForwardVector().X);
	float BoneX = Bone->GetActorLocation().X;
	float HeadX = CollisionHead->GetComponentLocation().X + (CollisionHead->GetScaledBoxExtent().X * Direction);		
	
	if (FMath::Abs(BoneX - HeadX) <= 10.f)
	{
		ForceStopMovement();
		Move(0.f);

		PlayerFounded = true;
		PlayerTarget = nullptr;

		if (bCanTakeItem)
		{
			bCanTakeItem = false;
			TakeItemPressed();
		}
		else
		{
			CurrentState = EDogState::Deactivated;
		}
	}
}
void ANpcDogPawn::CanComeBackHome()
{	
	if(bIsTakingItem)
	{
		ReturnToHome();
	}
}

void ANpcDogPawn::CanInteractWithObjects()
{
	Super::CanInteractWithObjects();

	if (Bone)
	{
		CheckBoneInsideTerritory();
	}

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

				SetActorTickEnabled(true);
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
		case EDogState::Deactivated:
			StateText = TEXT("Deactivated (Eating)");
			break;
		}
		FString FoundedText = PlayerFounded ? TEXT("TRUE (Found)") : TEXT("FALSE (Not Found)");
		FString TargetText = PlayerTarget ? FString::Printf(TEXT("VALID (%s)"), *PlayerTarget->GetName()) : TEXT("NULLPTR (Empty Memory)");
		FString VisibleText = CheckTargetVisible() ? TEXT("TRUE (Visible)") : TEXT("FALSE (Hidden)");
		FString EdgeText = IsAtLeashEdge() ? TEXT("TRUE (At Edge)") : TEXT("FALSE (Not At Edge)");
		FString BoneText = Bone ? FString::Printf(TEXT("VALID (%s)"), *Bone->GetName()) : TEXT("NULLPTR (Empty Bone)");
		FString TakingText = bIsTakingItem ? TEXT("TRUE (Bone in Mouth)") : TEXT("FALSE (Not At Edge)");
		
		GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Yellow, FString::Printf(TEXT("[Dog State]: %s"), *StateText));
		GEngine->AddOnScreenDebugMessage(2, 0.f, FColor::Cyan, FString::Printf(TEXT("[Player Founded]: %s"), *FoundedText));
		GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Orange, FString::Printf(TEXT("[Player Target]: %s"), *TargetText));
		GEngine->AddOnScreenDebugMessage(4, 0.f, FColor::Purple, FString::Printf(TEXT("[Target Visible]: %s"), *VisibleText));
		GEngine->AddOnScreenDebugMessage(5, 0.f, FColor::Green, FString::Printf(TEXT("[Is At Leash Edge]: %s"), *EdgeText));
		GEngine->AddOnScreenDebugMessage(6, 0.f, FColor::Red, FString::Printf(TEXT("[Bone Pointer]: %s"), *BoneText));
		GEngine->AddOnScreenDebugMessage(7, 0.f, FColor::Magenta, FString::Printf(TEXT("[Is Taking Item]: %s"), *TakingText));
	}


	switch (CurrentState)	
	{
	case EDogState::InitHome:
		ReturnToHome();
		if (CheckHomeLocation()) { TransitionToRepose(); }
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
		if (CheckHomeLocation()) { TransitionToRepose(); }
		else if (CheckTargetVisible()) { TransitionToAlert(); }
		break;
	case EDogState::Deactivated:
		CanComeBackHome();
		if (CheckHomeLocation()) { TransitionToDeactivated(); }
		break;
	default:
		break;
	}

	if (CheckBiting() && CurrentState != EDogState::Deactivated)
	{
		Biting();
	}
}
