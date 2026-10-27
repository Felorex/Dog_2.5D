// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseDogPawn.h"
#include "NpcDogPawn.generated.h"


class ADogPawn;

UENUM(BlueprintType)
enum class EDogState : uint8 
{
	InitHome UMETA(DisplayName = "Init Home"),
	Repose UMETA(DisplayName = "Repose"),
	Chase UMETA(DisplayName = "Chase"),
	Barking UMETA(DisplayName = "Barking"),
	Alert UMETA(DisplayName = "Alert"),
	Punished UMETA(DisplayName = "Punished"),
	ReturnToDoghouse UMETA(DisplayName = "Return to Doghouse"),
	Deactivated UMETA(DisplayName = "Deactivated")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBarkingState, bool, IsBarking);

UCLASS()
class DOG_2D_API ANpcDogPawn : public ABaseDogPawn
{
	GENERATED_BODY()

public:

	ANpcDogPawn();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, Category = "Dog|AI Events")
	FOnBarkingState OnBarkingState;

	UPROPERTY(BlueprintReadOnly, Category = "Dog|AI Status")
	EDogState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dog|AI Environment")
	TObjectPtr<AActor> TerritoryTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dog|AI Environment")
	TObjectPtr<AActor> TargetDisappeared;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dog|AI Setup")
	float TerritoryRadius;

	UFUNCTION(BlueprintImplementableEvent, Category = "Dog|AI Visuals")
	void OnBitingVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dog|AI Visuals")
	void OnBarkingVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dog|AI Visuals")
	void OnStopBarkingVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dog|AI Visuals")
	void OnPunishedVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dog|AI Visuals")
	void OnStopPunishedVisual();

	FVector DoghouseLocation;

	float GetBarkingTimer() const { return BarkingTimer; }

	void TransitionToPunished();

protected:

	virtual void BeginPlay() override;	
	virtual void CanInteractWithObjects() override;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Dog|AI Target")
	TObjectPtr<ADogPawn> PlayerTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|AI Balance")
	float HomeArrivalThreshold = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|AI Balance")
	float BonePickupDistance = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|AI Balance")
	float BitingDistance = 8.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|AI Balance")
	float AlertToChaseDelay = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|AI Balance")
	float LostTargetReturnDelay = 3.f;

	void CheckPlayerDetection();

	void StartToChase();
	void ChaseMovement();

	void StartBarking(float DeltaTime);
	void StartToAlert(float DeltaTime);

	void Biting();
	
	void TransitionToDeactivated();
	void TransitionToRepose();
	void TransitionToAlert();
	void TransitionToBarking();
	void TransitionToChase();	

	void ReturnToHome();

	void CheckBoneInsideTerritory();
	void CanComeBackHome();

	bool CheckHomeLocation() const;
	bool CheckTargetVisible() const;
	bool CheckBiting() const;
	bool CheckTargetInTriggerZone() const;

	float GetDistanceToTarget() const;
	float GetStopDistance() const;

	bool IsAtLeashEdge() const;

	bool bCanTakeItem;
	bool bPlayerFounded;
	bool bIsBarkingVisual;
	bool bIsPunishedVisual;

	float HomeX;
	float AlertTimer;
	float BarkingTimer;
};
