// Fill out your copyright notice in the Description page of Project Settings.


#include "HouseAlarmActor.h"
#include "NpcDogPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"


// Sets default values
AHouseAlarmActor::AHouseAlarmActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentHouseState = EHouseState::HouseSleep;

	IsLightActive = false;
	IsHumanInWindow = false;

	TargetDog = nullptr;

	InsideLightTimer = 0.f;
	HumanWatchTimer = 0.f;
	HumanOffLigthTimer = 0.f;
}

void AHouseAlarmActor::TransitionToHouseWokeUp()
{
	CurrentHouseState = EHouseState::HouseWokeUp;
}

bool AHouseAlarmActor::CanLightOn()
{
	if (IsLightActive || !TargetDog) return false;

	if (TargetDog->GetBarkingTimer() >= 5.f)
	{
		return true;
	}
	return false;
}
void AHouseAlarmActor::TransitionToLightOn()
{
	InsideLightTimer = 0.f;

	CurrentHouseState = EHouseState::LightOn;
}
void AHouseAlarmActor::LightIsOn(float DeltaTime)
{
	IsLightActive = true;

	InsideLightTimer += DeltaTime;

	if (WindowMesh && LightOnMaterial)
	{
		WindowMesh->SetMaterial(0, LightOnMaterial);
	}
}

bool AHouseAlarmActor::CanHumanWatch()
{
	if (InsideLightTimer >= 3.f && TargetDog && DogIsBarking)
	{
		return true;
	}
	return false;
}
bool AHouseAlarmActor::CanLightOffBeforeHuman()
{
	if (TargetDog && !DogIsBarking)
	{
		return true;
	}
	return false;
}
void AHouseAlarmActor::TransitionToHumanWatching()
{
	HumanWatchTimer = 0.f;
	CurrentHouseState = EHouseState::HumanWatching;
}
void AHouseAlarmActor::HumanWatchingInWindow(float DeltaTime)
{	
	if (!IsHumanInWindow)
	{
		IsHumanInWindow = true;		

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Human is watching in the window!"));
		}
	}
	HumanWatchTimer += DeltaTime;
}

void AHouseAlarmActor::TransitionToDogPunished()
{
	HumanWatchTimer = 0.f;
	CurrentHouseState = EHouseState::DogPunished;
}
void AHouseAlarmActor::DogGetPunished()
{
	if (TargetDog)
	{
		TargetDog->TransitionToPunished();
	}
}
bool AHouseAlarmActor::CanHumanLeft()
{
	if (!IsHumanInWindow) return false;

	if (HumanWatchTimer >= 2.f && TargetDog && !DogIsBarking)
	{
		return true;
	}
	return false;
}
bool AHouseAlarmActor::CanDogPunished()
{
	if (IsHumanInWindow && TargetDog && DogIsBarking)
	{
		return true;
	}
	return false;
}
void AHouseAlarmActor::TransitionToHumanLeft()
{
	HumanOffLigthTimer = 0.f;
	IsHumanInWindow = false;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Human left the window!"));
	}
	CurrentHouseState = EHouseState::HumanLeft;
}
void AHouseAlarmActor::HumanLeftWindow(float DeltaTime)
{	
	HumanOffLigthTimer += DeltaTime;
}

bool AHouseAlarmActor::CanLightOff()
{
	if (!IsLightActive) return false;
	if (HumanOffLigthTimer >= 2.f && TargetDog && !DogIsBarking)
	{
		return true;
	}
	return false;
}
void AHouseAlarmActor::TransitionToLightOff()
{
	CurrentHouseState = EHouseState::LightOff;
}
void AHouseAlarmActor::LightIsOff()
{
	IsLightActive = false;

	if (WindowMesh && LightOffMaterial)
	{
		WindowMesh->SetMaterial(0, LightOffMaterial);
	}
}

bool AHouseAlarmActor::CanHouseSleep()
{
	if (!IsLightActive && !IsHumanInWindow)
	{
		return true;
	}
	return false;
}
void AHouseAlarmActor::TransitionToHouseSleep()
{
	CurrentHouseState = EHouseState::HouseSleep;
	SetActorTickEnabled(false);
}

bool AHouseAlarmActor::DogAgainBarking()
{
	if (IsLightActive && TargetDog && DogIsBarking)
	{
		return true;
	}
	return false;
}

void AHouseAlarmActor::HandleDogBarkingState(bool IsBarking)
{
	DogIsBarking = IsBarking;

	if (IsBarking)
	{
		SetActorTickEnabled(true);
	}
}

// Called when the game starts or when spawned
void AHouseAlarmActor::BeginPlay()
{
	Super::BeginPlay();
	
	HouseMesh = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("House")));
	WindowMesh = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("Window")));
	LightZoneCollision = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("LightZone")));

	SetActorTickEnabled(false);

	if (!TargetDog)
	{
		TargetDog = Cast<ANpcDogPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), ANpcDogPawn::StaticClass()));
	}

	if (TargetDog)
	{
		TargetDog->OnBarkingState.AddDynamic(this, &AHouseAlarmActor::HandleDogBarkingState);
	}

	if (WindowMesh && LightOffMaterial)
	{
		WindowMesh->SetMaterial(0, LightOffMaterial);
	}
}

// Called every frame
void AHouseAlarmActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentHouseState)
	{	
	case EHouseState::HouseSleep:
		TransitionToHouseWokeUp();
		break;
	case EHouseState::HouseWokeUp:
		if (CanLightOn()) { TransitionToLightOn(); }
		break;
	case EHouseState::LightOn:
		LightIsOn(DeltaTime);
		if (CanHumanWatch()) { TransitionToHumanWatching();	}
		else if(CanLightOffBeforeHuman()) { TransitionToHumanLeft(); }
		break;
	case EHouseState::HumanWatching:
		HumanWatchingInWindow(DeltaTime);
		if (CanHumanLeft()) { TransitionToHumanLeft(); }
		else if (CanDogPunished()) { TransitionToDogPunished(); }
		break;
	case EHouseState::PlayerCaught:
		break;
	case EHouseState::DogPunished:
		DogGetPunished();
		if (CanLightOffBeforeHuman()) { TransitionToHumanLeft(); }
		break;
	case EHouseState::HumanLeft:
		HumanLeftWindow(DeltaTime);
		if (DogAgainBarking()) { TransitionToHumanWatching(); }
		else if (CanLightOff()) { TransitionToLightOff(); }
		break;
	case EHouseState::LightOff:
		LightIsOff();
		if (CanHouseSleep()) { TransitionToHouseSleep(); }
		break;
	default:
		break;
	}

}

