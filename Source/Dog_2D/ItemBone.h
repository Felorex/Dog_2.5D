// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBone.generated.h"


class ADogPawn;


UCLASS()
class DOG_2D_API AItemBone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBone();

	virtual void Tick(float DeltaTime) override;

	bool TryTake(ADogPawn* NewPlayer);
	void DisablePhysics();
	void EnablePhysics();

protected:

	virtual void BeginPlay() override;

	bool IsOnGround();
	void UpdatePhysics(float DeltaTime);

	UPROPERTY()
	UPrimitiveComponent* BoneComponent;

	UPROPERTY()
	ADogPawn* Player;

	float VelocityZ;
	float Gravity;
};
