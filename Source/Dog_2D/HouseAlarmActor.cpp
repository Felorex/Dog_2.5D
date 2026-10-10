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

	HouseMesh = CreateDefaultSubobject<UStaticMeshComponent>("HouseMesh");
	if (HouseMesh)
	{
		RootComponent = HouseMesh;
	}

	WindowMesh = CreateDefaultSubobject<UStaticMeshComponent>("WindowMesh");
	if (HouseMesh && WindowMesh)
	{
		WindowMesh->SetupAttachment(HouseMesh);
	}

	LightZoneCollision = CreateDefaultSubobject<UBoxComponent>("LightZoneCollision");
	if (WindowMesh && LightZoneCollision)
	{
		LightZoneCollision->SetupAttachment(WindowMesh);
	}
	
	ShadowHuman = CreateDefaultSubobject<UStaticMeshComponent>("ShadowHuman");
	if (WindowMesh && ShadowHuman)
	{
		ShadowHuman->SetupAttachment(WindowMesh);

		ShadowHuman->SetHiddenInGame(true);
		ShadowHuman->SetCastShadow(false);
	}

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
	if (!IsLightActive)
	{
		IsLightActive = true;
		OnHouseStateChange.Broadcast(true);
	}

	InsideLightTimer += DeltaTime;

	if (WindowMesh && LightOnMaterial)
	{
		WindowMesh->SetMaterial(0, LightOnMaterial);
	}
	if (LightZoneCollision)
	{
		LightZoneCollision->SetHiddenInGame(false);
	}	
}

bool AHouseAlarmActor::CanHumanWatch()
{
	if (InsideLightTimer >= 4.f && TargetDog && DogIsBarking)
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

		ShadowHuman->SetHiddenInGame(false);
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

	if (HumanWatchTimer >= 4.f && TargetDog && !DogIsBarking)
	{
		return true;
	}
	return false;
}
bool AHouseAlarmActor::CanDogPunished()
{
	if (IsHumanInWindow && HumanWatchTimer >= 4.f && TargetDog && DogIsBarking)
	{
		return true;
	}
	return false;
}
void AHouseAlarmActor::TransitionToHumanLeft()
{
	HumanOffLigthTimer = 0.f;
	IsHumanInWindow = false;

	ShadowHuman->SetHiddenInGame(true);
	CurrentHouseState = EHouseState::HumanLeft;
}
void AHouseAlarmActor::HumanLeftWindow(float DeltaTime)
{	
	HumanOffLigthTimer += DeltaTime;
}

bool AHouseAlarmActor::CanLightOff()
{
	if (!IsLightActive) return false;
	if (HumanOffLigthTimer >= 3.f && TargetDog && !DogIsBarking)
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
	if (IsLightActive)
	{
		IsLightActive = false;
		OnHouseStateChange.Broadcast(false);
	}

	if (WindowMesh && LightOffMaterial)
	{
		WindowMesh->SetMaterial(0, LightOffMaterial);
	}
	if (LightZoneCollision)
	{
		LightZoneCollision->SetHiddenInGame(true);
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
	if (LightZoneCollision)
	{
		LightZoneCollision->SetHiddenInGame(true);
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

