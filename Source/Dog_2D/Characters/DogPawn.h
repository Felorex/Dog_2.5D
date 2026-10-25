// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/BaseDogPawn.h"
#include "Interactive/StealthCover.h"

#include "DogPawn.generated.h"    

class AHouseAlarmActor;

UCLASS()
class DOG_2D_API ADogPawn : public ABaseDogPawn, public IStealthCoverInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADogPawn();	

	virtual void SetCurrentCover(class AStealthCover* NewCover) override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Move(float Value) override;
	virtual void DoJump() override;
	virtual void OnCrouchPressed() override;
	virtual void ForceStopMovement() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ineract_Movement")
	bool bIsInteracting;

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void InteractPressed();

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void InteractReleased();

	UFUNCTION(BlueprintImplementableEvent, Category = "InteractVisual")
	void OnPushVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "InteractVisual")
	void OnPullVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "InteractVisual")
	void OnStopInteractVisual();

	UFUNCTION(BlueprintCallable, Category = "Ineract_Movement")
	void SetMoveDirection(float Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dog|Scary Event")
	AActor* StopScaryEventZone;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Safe_Zone")
	AStealthCover* StealthCover;

	void ScaredRun(float DeltaTime);

	void SetIsScared(bool NewIsScared);

	bool GetIsScared() const;

	float GetVelocityX() const { return VelocityX; }
	float GetHeadEdgeX() const;
	float GetContainerForward() const;

	bool GetIsPulling() const { return bIsPulling; }
	bool GetIsPushing() const { return bIsPushing; }

protected:

	virtual void BeginPlay() override;
	virtual bool CanMoveWithHead(const FVector& Delta) override;
	virtual void GetAdditionalActorsToIgnore(TArray<AActor*>& Actors) const override;
	
	virtual float CalculateTargetY() const override;
	virtual void CanInteractWithObjects() override;


	UFUNCTION()
	void HandleHouseIsLightingState(bool bIsLighting);

	UPROPERTY()
	USceneComponent* CameraComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|Scary Event", meta = (BlueprintProtected = "true"))
	float ScarySpeed = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|Scary Event", meta = (BlueprintProtected = "true"))
	float ScaryStopDistanceThreshold = 10.f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Dog|Environment")
	TObjectPtr<AHouseAlarmActor> HouseAlarmSystem;

	void InteractMovementX();
	float CalculateAlignmentDeltaX() const;
	bool PreCheckAlignmentSpace();

	void HideInCover(float DeltaTime);
	bool CanHideInCover() const;

	float GetBodyEdgeY() const;

	bool bHouseIsLightingUp;
	bool bIsInCover;

	bool bIsPushing;
	bool bIsPulling;

	bool bIsScared;
	
};
