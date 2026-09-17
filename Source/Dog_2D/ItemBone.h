// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBone.generated.h"


class ABaseDogPawn;


UCLASS()
class DOG_2D_API AItemBone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBone();

	virtual void Tick(float DeltaTime) override;

	bool TryTake(ABaseDogPawn* NewBase);
	void DisablePhysics();
	void EnablePhysics();

protected:

	virtual void BeginPlay() override;

	bool IsOnGround();
	void UpdatePhysics(float DeltaTime);

	UPROPERTY()
	UPrimitiveComponent* BoneComponent;

	UPROPERTY()
	ABaseDogPawn* Base;

	float VelocityZ;
	float Gravity;
};
