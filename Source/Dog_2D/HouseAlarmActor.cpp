// Fill out your copyright notice in the Description page of Project Settings.


#include "HouseAlarmActor.h"
#include "NpcDogPawn.h"

// Sets default values
AHouseAlarmActor::AHouseAlarmActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	IsLightActive = false;
	IsHumanInWindow = false;

	TargetDog = nullptr;
}

// Called when the game starts or when spawned
void AHouseAlarmActor::BeginPlay()
{
	Super::BeginPlay();
	
	HouseMesh = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("House")));
	WindowMesh = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("Window")));
	LightZoneCollision = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("LightZone")));
}

// Called every frame
void AHouseAlarmActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

