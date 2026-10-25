// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
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

	bool GetIsBoxBlocked() { return bIsBoxBlocked; }

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Box|Physics")
	float GravityScale = 2000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Box|Physics")
	float GroundTracePadding = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Box|Physics")
	float BreakInteractFallSpeed = -300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Box|Debug")
	TEnumAsByte<EDrawDebugTrace::Type> BoxDebugTraceMode = EDrawDebugTrace::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Box|Components")
	TObjectPtr<UStaticMeshComponent> BoxComponent;

	UPROPERTY()
	TObjectPtr<ADogPawn> Player;

	bool bIsFollowing;
	bool bIsBoxBlocked;

	float BoxVelocityX;
	float BoxVelocityZ;	
};
