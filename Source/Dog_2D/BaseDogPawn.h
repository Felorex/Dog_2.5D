// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "BaseDogPawn.generated.h"

class AInteractiveBox;


UCLASS()
class DOG_2D_API ABaseDogPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseDogPawn();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Jump")
	void DoJump();

	UFUNCTION(BlueprintCallable, Category = "Jump")
	void StopJump();

	void ClearInteractiveBox();

	float GetBottomZ() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void UpdatePositionY(float DeltaTime);

	void UpdatePhysics(float DeltaTime);
	bool getIsGrounded() const { return IsGrounded; }
	void CheckGrounded();
	void Depenetration();

	void CheckBoxUnderfoot();
	void CheckJumpExecution();
	

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

	FTimerHandle CheckBoxTimer;
};
