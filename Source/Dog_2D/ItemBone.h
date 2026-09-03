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

protected:
	// Called when the game starts or when spawned
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Taken")
	UPrimitiveComponent* BoneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Taken")
	ADogPawn* Player;

	float VelocityZ;
	float VelocityX;
	float Gravity;

	bool IsTaking;
	bool IsFalling;

	bool IsOnGround();

	virtual void BeginPlay() override;

public:	
	// Called every frame

	bool TryTake(ADogPawn* NewPlayer);

	void UpdatePhysics(float DeltaTime);
	virtual void Tick(float DeltaTime) override;

};
