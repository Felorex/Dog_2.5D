// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "HouseAlarmActor.generated.h"

class ANpcDogPawn;

UENUM(BlueprintType)
enum class EHouseState : uint8
{
	HouseSleep UMETA(DisplayName = "House Sleep"),
	HouseWokeUp UMETA(DisplayName = "House Woke Up"),
	LightOn UMETA(DisplayName = "Light On"),
	HumanWatching UMETA(DisplayName = "Human Watching"),
	PlayerCaught UMETA(DisplayName = "Player Caught"),
	DogPunished UMETA(DisplayName = "Dog Punished"),
	HumanLeft UMETA(DisplayName = "Human Left"),
	LightOff UMETA(DisplayName = "Light Off")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHouseStateChange, bool, bIsLighting);

UCLASS()
class DOG_2D_API AHouseAlarmActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHouseAlarmActor();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, Category = "House State")
	FOnHouseStateChange OnHouseStateChange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Connection")
	ANpcDogPawn* TargetDog;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleDogBarkingState(bool IsBarking);

	UPROPERTY()
	UStaticMeshComponent* HouseMesh;

	UPROPERTY()
	UStaticMeshComponent* WindowMesh;

	UPROPERTY()
	UBoxComponent* LightZoneCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House State")
	EHouseState CurrentHouseState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light Settings")
	UMaterialInterface* LightOnMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light Settings")
	UMaterialInterface* LightOffMaterial;


	void TransitionToHouseWokeUp();
	void TransitionToLightOn();
	void TransitionToHumanWatching();
	void TransitionToHumanLeft();
	void TransitionToLightOff();
	void TransitionToHouseSleep();
	void TransitionToDogPunished();

	void LightIsOn(float DeltaTime);
	void LightIsOff();

	void HumanWatchingInWindow(float DeltaTime);
	void HumanLeftWindow(float DeltaTime);

	void DogGetPunished();

	bool DogAgainBarking();

	bool CanLightOn();
	bool CanLightOff();
	bool CanLightOffBeforeHuman();
	bool CanHumanWatch();
	bool CanHumanLeft();
	bool CanDogPunished();
	bool CanHouseSleep();

	bool IsLightActive;
	bool IsHumanInWindow;

	bool DogIsBarking;
	
	float InsideLightTimer;
	float HumanWatchTimer;
	float HumanOffLigthTimer;
};
