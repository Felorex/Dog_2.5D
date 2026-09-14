// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "BaseDogPawn.generated.h"

UCLASS()
class DOG_2D_API ABaseDogPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseDogPawn();

	virtual void Tick(float DeltaTime) override;

	float GetVelocityX() const { return VelocityX; }
	float GetHeadEdgeX() const;
	float GetHeadEdgeY() const;
	float GetHeadEdgeZ() const;
	float GetBottomZ() const;
	float GetContainerForward() const { return Conteiner ? Conteiner->GetForwardVector().X : 1.0f; }
	bool GetIsPulling() const { return bIsPulling; }
	bool GetIsPushing() const { return bIsPushing; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void UpdatePhysics(float DeltaTime);
	bool getIsGrounded() const { return IsGrounded; }
	void CheckGrounded();
	void Depenetration();

	bool CanMoveWithHead(float DeltaX);

	USceneComponent* Conteiner;
	USceneComponent* MouthComp;
	USceneComponent* CameraComp;
	UBoxComponent* CollisionBody;
	UBoxComponent* CollisionHead;

	float OriginalExtentBodyZ;
	float OriginalExtentHeadZ;

	bool bWantToJump;
	bool IsJumping;
	float JumpForce;

	float Gravity;
	bool IsGrounded;
	float OriginalY;
	float TargetY;

	bool bWantToCrouch;
	float CrouchSpeed;
	bool IsCrouching;

	float InteractDistance;
	float VelocityZ;
	float VelocityX;
	float MoveSpeed;

	bool bIsPushing;
	bool bIsPulling;

	bool bIsTakingItem;
};
