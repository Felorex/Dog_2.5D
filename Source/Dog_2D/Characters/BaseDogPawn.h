// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
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

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void ForceStopMovement();

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

	virtual float CalculateTargetY() const;

	virtual void CanInteractWithObjects();
	FHitResult InteractHitResult;

	void UpdatePhysics(float DeltaTime);
	bool getIsGrounded() const { return bIsGrounded; }
	void CheckGrounded();
	void Depenetration();

	void ApplyPositionIntrolationY(float DeltaTime);

	void CheckBoxUnderfoot();
	void CheckJumpExecution();
	
	void TryStandUp();
	bool CanStandUp();

	bool WantToTakeItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dog|Components")
	TObjectPtr<USceneComponent> VisualContainer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dog|Components")
	TObjectPtr<USceneComponent> MouthComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dog|Components")
	TObjectPtr<UBoxComponent> CollisionBody;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dog|Components")
	TObjectPtr<UBoxComponent> CollisionHead;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dog|Components")
	TObjectPtr<UStaticMeshComponent> Head;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|Movement", meta = (BlueprintProtected = "true"))
	float MoveSpeed = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|Movement", meta = (BlueprintProtected = "true"))
	float CrouchSpeed = 150.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|Movement", meta = (BlueprintProtected = "true"))
	float BoxAlignmentZOffset = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|Movement", meta = (BlueprintProtected = "true"))
	float YInterpSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|Physics", meta = (BlueprintProtected = "true"))
	float JumpForce = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|Physics", meta = (BlueprintProtected = "true"))
	float Gravity = -980.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|Interaction", meta = (BlueprintProtected = "true"))
	float InteractDistance = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dog|Debug")
	TEnumAsByte<EDrawDebugTrace::Type> DogDebugTraceMode = EDrawDebugTrace::None;

	UPROPERTY()
	TObjectPtr<AInteractiveBox> Box;
	UPROPERTY()
	TObjectPtr<AItemBone> Bone;

	float OriginalExtentBodyZ;
	float OriginalExtentHeadZ;

	float OriginalY;
	float TargetY;

	bool bWantToJump;
	bool bIsJumping;
	bool bIsGrounded;
	bool bWantToCrouch;
	bool bIsCrouching;
	bool bIsTakingItem;

	float VelocityZ;
	float VelocityX;

	FTimerHandle CheckBoxTimer;
	FTimerHandle InteractTimer;
};
