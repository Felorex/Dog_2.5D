// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "BaseDogPawn.generated.h"

class AInteractiveBox;
class AItemBone;


UCLASS()
class DOG_2D_API ABaseDogPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseDogPawn();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void Move(float Value);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void StopMove();

	UFUNCTION(BlueprintCallable, Category = "Jump")
	virtual void DoJump();

	UFUNCTION(BlueprintCallable, Category = "Jump")
	void StopJump();

	UFUNCTION(BlueprintCallable, Category = "Crouch")
	virtual void OnCrouchPressed();

	UFUNCTION(BlueprintCallable, Category = "Stand")
	void OnCrouchReleased();

	UFUNCTION(BlueprintCallable, Category = "TakeItem")
	void TakeItemPressed();

	UFUNCTION(BlueprintCallable, Category = "TakeItem")
	void AttachItemToMouth();

	UFUNCTION(BlueprintCallable, Category = "TakeItem")
	void DetachItemFromMouth();

	UFUNCTION(BlueprintImplementableEvent, Category = "StandVisual")
	void OnLookRightVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "StandVisual")
	void OnLookLeftVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "Crouch")
	void OnCrouchVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "Stand")
	void OnStandVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "TakeVisual")
	void OnPickupVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "TakeVisual")
	void OnDropVisual();

	virtual void ForceStopMovement();

	void ClearInteractiveBox();
	void ClearItemBone();

	float GetBottomZ() const;

	float GetMinCollisionX() const;
	float GetMaxCollisionX() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void UpdatePositionY(float DeltaTime);

	virtual bool CanMoveWithHead(float DeltaX);

	virtual void CanInteractWithObjects();
	FHitResult InteractHitResult;

	void UpdatePhysics(float DeltaTime);
	bool getIsGrounded() const { return IsGrounded; }
	void CheckGrounded();
	void Depenetration();
	
	void CheckBoxUnderfoot();
	void CheckJumpExecution();
	
	void TryStandUp();
	bool CanStandUp();

	bool WantToTakeItem();

	UPROPERTY()
	USceneComponent* Conteiner;
	UPROPERTY()
	USceneComponent* MouthComp;
	UPROPERTY()
	UBoxComponent* CollisionBody;
	UPROPERTY()
	UBoxComponent* CollisionHead;

	UPROPERTY()
	AInteractiveBox* Box;
	UPROPERTY()
	AItemBone* Bone;

	float OriginalExtentBodyZ;
	float OriginalExtentHeadZ;

	float OriginalY;
	float TargetY;

	bool bWantToJump;
	bool IsJumping;
	float JumpForce;

	float Gravity;
	bool IsGrounded;

	bool bWantToCrouch;
	float CrouchSpeed;
	bool IsCrouching;

	float VelocityZ;
	float VelocityX;
	float MoveSpeed;

	bool bIsTakingItem;

	float InteractDistance;


	FTimerHandle CheckBoxTimer;
	FTimerHandle InteractTimer;
};
