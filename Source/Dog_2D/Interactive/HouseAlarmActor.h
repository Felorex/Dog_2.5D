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

	UPROPERTY(BlueprintAssignable, Category = "House|House Events")
	FOnHouseStateChange OnHouseStateChange;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "House|AI Connection")
	TObjectPtr<ANpcDogPawn> TargetDog;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleDogBarkingState(bool bIsBarking);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "House|Components")
	TObjectPtr<UStaticMeshComponent> HouseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "House|Components")
	TObjectPtr<UStaticMeshComponent> WindowMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "House|Components")
	TObjectPtr<UBoxComponent> LightZoneCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "House|Components")
	TObjectPtr<UStaticMeshComponent> ShadowHuman;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House|House State")
	EHouseState CurrentHouseState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House|Light Settings")
	TObjectPtr<UMaterialInterface> LightOnMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House|Light Settings")
	TObjectPtr<UMaterialInterface> LightOffMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "House|Balance")
	float BarkTimeToWakeUp = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "House|Balance")
	float LightOnBeforeWatchDuration = 4.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "House|Balance")
	float HumanWatchingDuration = 4.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "House|Balance")
	float LightOffDelay = 3.f;

	UFUNCTION()
	void OnLightZoneBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnLightZoneEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void TransitionToHouseWokeUp();
	void TransitionToLightOn();
	void TransitionToHumanWatching();
	void TransitionToHumanLeft();
	void TransitionToLightOff();
	void TransitionToHouseSleep();
	void TransitionToDogPunished();
	void TransitionToPlayerCaught();

	void LightIsOn(float DeltaTime);
	void LightIsOff();
	void HumanWatchingInWindow(float DeltaTime);
	void HumanLeftWindow(float DeltaTime);
	void DogGetPunished();
	void PlayerGetCaught();

	bool DogAgainBarking();

	bool CanLightOn();
	bool CanLightOff();
	bool CanLightOffBeforeHuman();
	bool CanHumanWatch();
	bool CanHumanLeft();
	bool CanDogPunished();
	bool CanHouseSleep();
	bool CanPlayerCaught();

	bool bIsLightActive;
	bool bIsHumanInWindow;

	bool bHasPlayerBeenCaught;

	bool bDogIsBarking;
	bool bPlayerInsideLight;	
	
	float InsideLightTimer;
	float HumanWatchTimer;
	float HumanOffLightTimer;
};
