// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveBox.generated.h"

class ADogPawn;


UCLASS()
class DOG_2D_API AInteractiveBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractiveBox();

	virtual void Tick(float DeltaTime) override;

	void StartFollow(ADogPawn* NewPlayer);
	void StopFollow();

	bool GetIsBoxBlocked() { return IsBoxBlocked; }

	float GetBoxEdgeY() const;
	float GetBoxLeftEdgeX() const;
	float GetBoxRightEdgeX() const;
	float GetBoxEdgeZ() const;

protected:

	virtual void BeginPlay() override;

	bool IsOnGround();
	void UpdatePhysics(float DeltaTime);
	void UpdateMovementX();
	void CheckWallCollision(const FHitResult& Hit);

	UPROPERTY()
	ADogPawn* Player;

	UPROPERTY()
	UPrimitiveComponent* BoxComponent;

	bool bIsFollowing;
	bool IsBoxBlocked;

	float BoxVelocityX;
	float BoxVelocityZ;
	float GravityScale;
};
