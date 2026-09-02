// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBone.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AItemBone::AItemBone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VelocityZ = 0.0f;
	VelocityX = 0.f;
	Gravity = -980.0f;
}

bool AItemBone::IsOnGround()
{
	if (!BoneComponent) return false;

	FVector Start = BoneComponent->GetComponentLocation();
	float HalfHeight = BoneComponent->Bounds.BoxExtent.Z;
	FVector End = Start - FVector(0.0f, 0.0f, HalfHeight + 1.0f);
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	bool bHit = UKismetSystemLibrary::LineTraceSingle(
		this,
		Start,
		End,
		UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true
	);
	return bHit && HitResult.IsValidBlockingHit();
}

void AItemBone::UpdatePhysics(float DeltaTime)
{
	// Implementation for physics updates
	if (!BoneComponent) return;

	if (!IsOnGround())
	{
		VelocityZ += Gravity * DeltaTime;
	}
	else
	{
		VelocityZ = 0.0f;
	}
}

// Called when the game starts or when spawned
void AItemBone::BeginPlay()
{
	Super::BeginPlay();
	
	BoneComponent = Cast<UPrimitiveComponent>(GetRootComponent());
}

// Called every frame
void AItemBone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePhysics(DeltaTime);

	float DeltaZ = VelocityZ * DeltaTime;

	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, FString::Printf(TEXT("VelocityZ: %f, CurrentZ: %f"), VelocityZ, GetActorLocation().Z));
	AddActorWorldOffset(FVector(0.0f, 0.0f, DeltaZ), true);
}

