// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ItemBone.generated.h"


UCLASS()
class DOG_2D_API AItemBone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBone();

	virtual void Tick(float DeltaTime) override;

	bool CanTakeItem(AActor* IgnoreActor);
	void DisablePhysics();
	void EnablePhysics();

protected:

	virtual void BeginPlay() override;

	bool IsOnGround();
	void UpdatePhysics(float DeltaTime);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone|Physics")
	float Gravity = -980.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone|Physics")
	float GroundTracePadding = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone|Sensing")
	float TakeTraceStartOffset = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone|Sensing")
	float TakeTraceLength = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone|Debug")
	TEnumAsByte<EDrawDebugTrace::Type> BoneDebugTraceMode = EDrawDebugTrace::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bone|Components")
	TObjectPtr<UStaticMeshComponent> BoneComponent;

	float VelocityZ;
};
