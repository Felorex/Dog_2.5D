// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDogPawn.h"
#include "NpcDogPawn.generated.h"


class ADogPawn;

UENUM(BlueprintType)
enum class EDogState : uint8 
{
	Repose UMETA(DisplayName = "Repose"),
	Chase UMETA(DisplayName = "Chase"),
	Barking UMETA(DisplayName = "Barking"),
	Alert UMETA(DisplayName = "Alert"),
	ReturnToDoghouse UMETA(DisplayName = "Return to Doghouse")
};

UCLASS()
class DOG_2D_API ANpcDogPawn : public ABaseDogPawn
{
	GENERATED_BODY()

public:

	ANpcDogPawn();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = "Dog_AI")
	EDogState CurrentState;

	UPROPERTY(BlueprintReadOnly, Category = "Dog_AI")
	ADogPawn* PlayerTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dog_AI")
	AActor* TerritoryTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dog_AI")
	float TerritoryRadius;

	FVector DoghouseLocation;


protected:

	virtual void BeginPlay() override;
	
	virtual void CanInteractWithObjects() override;

	void StartToChase();

	void ChaseMovement();

	void CheckHomeLocation();

	bool PlayerFounded;

	float HomeX;
};
