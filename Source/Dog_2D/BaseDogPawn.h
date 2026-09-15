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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	USceneComponent* Conteiner;
	UPROPERTY()
	USceneComponent* MouthComp;
	UPROPERTY()
	UBoxComponent* CollisionBody;
	UPROPERTY()
	UBoxComponent* CollisionHead;

	float OriginalExtentBodyZ;
	float OriginalExtentHeadZ;

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

};
