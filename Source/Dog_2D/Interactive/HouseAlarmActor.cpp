// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactive/HouseAlarmActor.h"
#include "Characters/NpcDogPawn.h"
#include "Characters/DogPawn.h"
#include "GameModes/DogGameMode.h"
#include "Kismet/GameplayStatics.h"


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

	bIsLightActive = false;
	bIsHumanInWindow = false;

	bHasPlayerBeenCaught = false;

	InsideLightTimer = 0.f;
	HumanWatchTimer = 0.f;
	HumanOffLightTimer = 0.f;
}

void AHouseAlarmActor::OnLightZoneBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))
	{
		APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (OtherActor == Player)
		{
			bPlayerInsideLight = true;
		}
	}
}
void AHouseAlarmActor::OnLightZoneEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this))
	{
		APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (OtherActor == Player)
		{
			bPlayerInsideLight = false;
		}
	}
}
void AHouseAlarmActor::TransitionToHouseWokeUp()
{
	bHasPlayerBeenCaught = false;
	CurrentHouseState = EHouseState::HouseWokeUp;
}

bool AHouseAlarmActor::CanLightOn()
{
	if (bIsLightActive || !TargetDog) return false;

	return (TargetDog->GetBarkingTimer() >= BarkTimeToWakeUp);
}
void AHouseAlarmActor::TransitionToLightOn()
{
	InsideLightTimer = 0.f;
	CurrentHouseState = EHouseState::LightOn;
}
void AHouseAlarmActor::LightIsOn(float DeltaTime)
{
	if (!bIsLightActive)
	{
		bIsLightActive = true;
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
	return (InsideLightTimer >= LightOnBeforeWatchDuration && TargetDog && bDogIsBarking);
}
bool AHouseAlarmActor::CanLightOffBeforeHuman()
{
	return (TargetDog && !bDogIsBarking);
}
void AHouseAlarmActor::TransitionToHumanWatching()
{
	HumanWatchTimer = 0.f;
	CurrentHouseState = EHouseState::HumanWatching;
}
void AHouseAlarmActor::HumanWatchingInWindow(float DeltaTime)
{	
	if (!bIsHumanInWindow)
	{
		bIsHumanInWindow = true;
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
	if (!bIsHumanInWindow) return false;

	return (HumanWatchTimer >= HumanWatchingDuration && TargetDog && !bDogIsBarking);
}
bool AHouseAlarmActor::CanDogPunished()
{
	return (bIsHumanInWindow && HumanWatchTimer >= HumanWatchingDuration && TargetDog && bDogIsBarking);
}

void AHouseAlarmActor::TransitionToPlayerCaught()
{
	HumanWatchTimer = 0.f;
	CurrentHouseState = EHouseState::PlayerCaught;
}
bool AHouseAlarmActor::CanPlayerCaught()
{
	return (bIsHumanInWindow && HumanWatchTimer >= HumanWatchingDuration && bPlayerInsideLight);
}
void AHouseAlarmActor::PlayerGetCaught()
{
	if (bHasPlayerBeenCaught) return;

	if (GetWorld())
	{
		ADogGameMode* GameMode = Cast<ADogGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->OnPlayerCaught();
			bHasPlayerBeenCaught = true;
		}
	}
}

void AHouseAlarmActor::TransitionToHumanLeft()
{
	HumanOffLightTimer = 0.f;
	bIsHumanInWindow = false;

	ShadowHuman->SetHiddenInGame(true);
	CurrentHouseState = EHouseState::HumanLeft;
}
void AHouseAlarmActor::HumanLeftWindow(float DeltaTime)
{	
	HumanOffLightTimer += DeltaTime;
}

bool AHouseAlarmActor::CanLightOff()
{
	if (!bIsLightActive) return false;
	return (HumanOffLightTimer >= LightOffDelay && TargetDog && !bDogIsBarking);
}
void AHouseAlarmActor::TransitionToLightOff()
{
	CurrentHouseState = EHouseState::LightOff;
}
void AHouseAlarmActor::LightIsOff()
{
	if (bIsLightActive)
	{
		bIsLightActive = false;
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
	return (!bIsLightActive && !bIsHumanInWindow);
}
void AHouseAlarmActor::TransitionToHouseSleep()
{
	CurrentHouseState = EHouseState::HouseSleep;
	SetActorTickEnabled(false);
}

bool AHouseAlarmActor::DogAgainBarking()
{
	return (bIsLightActive && TargetDog && bDogIsBarking);
}

void AHouseAlarmActor::HandleDogBarkingState(bool bIsBarking)
{
	bDogIsBarking = bIsBarking;

	if (bIsBarking)
	{
		SetActorTickEnabled(true);
	}
}

// Called when the game starts or when spawned
void AHouseAlarmActor::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);

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

		LightZoneCollision->OnComponentBeginOverlap.AddDynamic(this, &AHouseAlarmActor::OnLightZoneBeginOverlap);
		LightZoneCollision->OnComponentEndOverlap.AddDynamic(this, &AHouseAlarmActor::OnLightZoneEndOverlap);
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
		if (CanPlayerCaught()) { TransitionToPlayerCaught(); }		
		else if (CanHumanLeft()) { TransitionToHumanLeft(); }
		else if (CanDogPunished()) { TransitionToDogPunished(); }
		break;
	case EHouseState::PlayerCaught:
		PlayerGetCaught();
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

