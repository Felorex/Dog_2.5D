// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDogPawn.h"
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

	UPROPERTY(BlueprintAssignable, Category = "Dog_AI")
	FOnBarkingState OnBarkingState;

	UPROPERTY(BlueprintReadOnly, Category = "Dog_AI")
	EDogState CurrentState;

	UPROPERTY(BlueprintReadOnly, Category = "Dog_AI")
	ADogPawn* PlayerTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dog_AI")
	AActor* TerritoryTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dog_AI")
	AActor* TargetDisappeared;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dog_AI")
	float TerritoryRadius;

	UFUNCTION(BlueprintImplementableEvent, Category = "BiteVisual")
	void OnBitingVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "BarkVisual")
	void OnBarkingVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "BarkVisual")
	void OnStopBarkingVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "PunishedVisual")
	void OnPunishedVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "PunishedVisual")
	void OnStopPunishedVisual();

	FVector DoghouseLocation;

	float GetBarkingTimer() const { return BarkingTimer; }

	void TransitionToPunished();

protected:

	virtual void BeginPlay() override;
	
	virtual void CanInteractWithObjects() override;

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
	bool PlayerFounded;
	bool IsBarkingVisual;
	bool IsPunishedVisual;

	float HomeX;
	float AlertTimer;
	float BarkingTimer;
};
