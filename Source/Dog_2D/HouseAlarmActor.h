// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "HouseAlarmActor.generated.h"

class ANpcDogPawn;

UCLASS()
class DOG_2D_API AHouseAlarmActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHouseAlarmActor();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Connection")
	ANpcDogPawn* TargetDog;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	UStaticMeshComponent* HouseMesh;

	UPROPERTY()
	UStaticMeshComponent* WindowMesh;

	UPROPERTY()
	UBoxComponent* LightZoneCollision;


	bool IsLightActive;
	bool IsHumanInWindow;
	
};
