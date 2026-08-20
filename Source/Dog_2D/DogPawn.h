// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"

#include "InteractiveBox.h"

#include "DogPawn.generated.h"



UCLASS()
class DOG_2D_API ADogPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADogPawn();	

protected:

	AInteractiveBox* Box;

	USceneComponent* Conteiner;

	USceneComponent* CameraComp;

	UBoxComponent* CollisionBody;
	UBoxComponent* CollisionHead;
	float OriginalExtentBodyZ;
	float OriginalExtentHeadZ;
	float CrouchedScaleBodyZ;
	float CrouchedScaleHeadZ;

	bool bWantToJump;
	float OriginalY;
	float TargetY;

	float Gravity;
	bool IsGrounded;
	bool IsCrouching;	
	bool IsJumping;
	float InteractDistance;
	float VelocityZ;
	float VelocityX;
	float MoveSpeed;
	float CrouchSpeed;
	float JumpForce;


	FTimerHandle InteractTimer;
	FTimerHandle CheckBoxTimer;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

public:	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ineract_HeadAnimation")
	bool bIsPushing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ineract_HeadAnimation")
	bool bIsPulling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ineract_Movement")
	bool bIsInteracting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crouch")
	bool bWantToCrouch;

	UFUNCTION(BlueprintCallable, Category = "Ineract_Movement")
	void SetMoveDirection(float Value);

	UFUNCTION(BlueprintCallable, Category = "Grounded")
	bool getIsGrounded() const { return IsGrounded; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Move(float Value);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopMove();

	UFUNCTION(BlueprintCallable, Category = "Jump")
	void DoJump();

	UFUNCTION(BlueprintCallable, Category = "Jump")
	void StopJump();

	UFUNCTION(BlueprintCallable, Category = "Crouch")
	void OnCrouchPressed();

	UFUNCTION(BlueprintImplementableEvent, Category = "Crouch")
	void OnCrouchVisual();

	UFUNCTION(BlueprintCallable, Category = "Stand")
	void OnCrouchReleased();

	UFUNCTION(BlueprintImplementableEvent, Category = "Stand")
	void OnStandVisual();

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

	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void UpdatePhysics(float DeltaTime);
	
	bool CanMoveWithHead(float DeltaX);

	void CheckGrounded();

	void UpdatePositionY(float DeltaTime);
	void InteractMovementX();
	void CanInteractWithBox();
	void CheckBoxUnderfoot();	
	void CheckJumpExecution();

	void Depenetration();
	void DepenetrationZ();
	
	void DoCrouch();
	void DoStand();
	bool CanStandUp();
	void TryStandUp();

	float GetVelocityX() const { return VelocityX; }
	float GetHeadEdgeX() const;
	float GetHeadEdgeY() const;
	float GetHeadEdgeZ() const;
	float GetBottomZ() const;
	float GetContainerForward() const { return Conteiner ? Conteiner->GetForwardVector().X : 1.0f; }
	bool GetIsPulling() const { return bIsPulling; }
	bool GetIsPushing() const { return bIsPushing; }

	void ForceStopMovement();
	void ClearInteractiveBox();
	
};
