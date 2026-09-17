// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"

#include "BaseDogPawn.h"
#include "ItemBone.h"

#include "DogPawn.generated.h"



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
	virtual void StopMove() override;

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

	float GetVelocityX() const { return VelocityX; }
	float GetHeadEdgeX() const;
	float GetHeadEdgeY() const;
	float GetHeadEdgeZ() const;
	bool GetIsPulling() const { return bIsPulling; }
	bool GetIsPushing() const { return bIsPushing; }
	float GetContainerForward() const;

protected:

	virtual void BeginPlay() override;
	virtual void UpdatePositionY(float DeltaTime) override;
	virtual bool CanMoveWithHead(float DeltaX) override;
	
	virtual void CanInteractWithObjects() override;

	void InteractMovementX();
	float CalculateAlignmentDeltaX() const;
	bool PreCheckAlignmentSpace();

	USceneComponent* CameraComp;
		
	bool bIsPushing;
	bool bIsPulling;
};
