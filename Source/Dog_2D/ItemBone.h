// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBone.generated.h"

UCLASS()
class DOG_2D_API AItemBone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBone();

protected:
	// Called when the game starts or when spawned
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Follow")
	UPrimitiveComponent* BoneComponent;

	float VelocityZ;
	float VelocityX;

	bool IsOnGround();

	virtual void BeginPlay() override;

public:	
	// Called every frame
	void UpdatePhysics(float DeltaTime);
	virtual void Tick(float DeltaTime) override;

};
