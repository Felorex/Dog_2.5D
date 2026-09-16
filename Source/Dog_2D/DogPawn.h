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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ineract_Movement")
	bool bIsInteracting;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Move(float Value);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopMove();

	//UFUNCTION(BlueprintCallable, Category = "Jump")
	//void DoJump();

	//UFUNCTION(BlueprintCallable, Category = "Jump")
	//void StopJump();

	UFUNCTION(BlueprintCallable, Category = "Crouch")
	void OnCrouchPressed();

	UFUNCTION(BlueprintCallable, Category = "Stand")
	void OnCrouchReleased();

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void InteractPressed();

	UFUNCTION(BlueprintCallable, Category = "Interact")
	void InteractReleased();

	UFUNCTION(BlueprintCallable, Category = "TakeItem")
	void TakeItemPressed();

	UFUNCTION(BlueprintImplementableEvent, Category = "Crouch")
	void OnCrouchVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "Stand")
	void OnStandVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "StandVisual")
	void OnLookRightVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "StandVisual")
	void OnLookLeftVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "InteractVisual")
	void OnPushVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "InteractVisual")
	void OnPullVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "InteractVisual")
	void OnStopInteractVisual();

	UFUNCTION(BlueprintCallable, Category = "Ineract_Movement")
	void SetMoveDirection(float Value);

	UFUNCTION(BlueprintImplementableEvent, Category = "TakeVisual")
	void OnPickupVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "TakeVisual")
	void OnDropVisual();

	UFUNCTION(BlueprintCallable, Category = "TakeItem")
	void AttachItemToMouth();

	UFUNCTION(BlueprintCallable, Category = "TakeItem")
	void DetachItemFromMouth();

	float GetVelocityX() const { return VelocityX; }
	float GetHeadEdgeX() const;
	float GetHeadEdgeY() const;
	float GetHeadEdgeZ() const;
	bool GetIsPulling() const { return bIsPulling; }
	bool GetIsPushing() const { return bIsPushing; }
	float GetContainerForward() const;

	void ForceStopMovement();
	void ClearItemBone();

protected:

	virtual void BeginPlay() override;
	virtual void UpdatePositionY(float DeltaTime) override;

	bool CanMoveWithHead(float DeltaX);
	
	void InteractMovementX();
	void CanInteractWithObjects();
	/*void CheckBoxUnderfoot();
	void CheckJumpExecution();*/

	bool CanStandUp();
	void TryStandUp();

	float CalculateAlignmentDeltaX() const;
	bool PreCheckAlignmentSpace();
	bool WantToTakeItem();

	AItemBone* Bone;

	USceneComponent* CameraComp;

	//float OriginalY;
	//float TargetY;
	
	float InteractDistance;
	
	bool bIsPushing;
	bool bIsPulling;

	bool bIsTakingItem;

	FTimerHandle InteractTimer;
	/*FTimerHandle CheckBoxTimer;*/
};
