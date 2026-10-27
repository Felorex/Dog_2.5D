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
class AStealthCover;

UCLASS()
class DOG_2D_API ADogPawn : public ABaseDogPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADogPawn();	

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Move(float Value) override;
	virtual void DoJump() override;
	virtual void OnCrouchPressed() override;
	virtual void ForceStopMovement() override;	

	UFUNCTION(BlueprintCallable, Category = "Dog|Interact")
	void InteractPressed();

	UFUNCTION(BlueprintCallable, Category = "Dog|Interact")
	void InteractReleased();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dog|InteractVisual")
	void OnPushVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dog|InteractVisual")
	void OnPullVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dog|InteractVisual")
	void OnStopInteractVisual();

	UFUNCTION(BlueprintCallable, Category = "Dog|Movement")
	void SetMoveDirection(float Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dog|Scary Event")
	TObjectPtr<AActor> StopScaryEventZone;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dog|Stealth")
	TObjectPtr<AStealthCover> StealthCover;

	void SetCurrentCover(AStealthCover* NewCover);

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
	virtual void UpdateMovementVisual(float Value) override;
	virtual void GetAdditionalActorsToIgnore(TArray<AActor*>& Actors) const override;
	
	virtual float CalculateTargetY() const override;
	virtual void CanInteractWithObjects() override;


	UFUNCTION()
	void HandleHouseIsLightingState(bool bIsLighting);

	UPROPERTY()
	TObjectPtr<USceneComponent> CameraComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|Scary Event")
	float ScarySpeed = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|Scary Event")
	float ScaryStopDistanceThreshold = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|Interact")
	float AlignmentTolerance = 1.f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Dog|Environment")
	TObjectPtr<AHouseAlarmActor> HouseAlarmSystem;

	void InteractMovementX();
	float CalculateAlignmentDeltaX() const;
	bool PreCheckAlignmentSpace();

	void HideInCover(float DeltaTime);
	bool CanHideInCover() const;

	float GetBodyEdgeY() const;

	bool bIsInteracting;

	bool bHouseIsLightingUp;
	bool bIsInCover;

	bool bIsPushing;
	bool bIsPulling;

	bool bIsScared;	
};
