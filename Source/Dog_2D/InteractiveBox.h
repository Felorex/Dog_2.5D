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

protected:

	float BoxVelocityX;
	float BoxVelocityY;
	float BoxVelocityZ;

	float GravityScale;
	float MaxFallSpeed;

	bool IsBoxBlocked;
	bool LastMoving;

	bool IsOnGround();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Follow")
	ADogPawn* Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Follow")
	bool bIsFollowing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Follow")
	float OriginalY;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Follow")
	UPrimitiveComponent* BoxComponent;


public:	

	UFUNCTION(BlueprintCallable, Category = "Follow")
	void StartFollow(ADogPawn* NewPlayer);

	UFUNCTION(BlueprintCallable, Category = "Follow")
	void StopFollow();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void UpdatePhysics(float DeltaTime);
	void UpdateMovementX();
	void CheckWallCollision(const FHitResult& Hit);
	bool GetIsBoxBlocked() { return IsBoxBlocked; }

	float GetBoxEdgeY() const;
	float GetBoxLeftEdgeX() const;
	float GetBoxRightEdgeX() const;
	float GetBoxEdgeZ() const;
};
