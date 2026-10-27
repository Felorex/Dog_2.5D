// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactive/ItemBone.h"
#include "Components/BoxComponent.h"



// Sets default values
AItemBone::AItemBone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoneComponent = CreateDefaultSubobject<UStaticMeshComponent>("BoneComponent");
	if (BoneComponent)
	{
		RootComponent = BoneComponent;
	}

	VelocityZ = 0.0f;
}

bool AItemBone::IsOnGround()
{
	if (!BoneComponent) return false;

	FVector Start = BoneComponent->GetComponentLocation();

	FVector Origin;
	FVector BoxExtent;
	GetActorBounds(false, Origin, BoxExtent);
	float HalfHeight = BoxExtent.Z;

	FVector End = Start - FVector(0.0f, 0.0f, HalfHeight + GroundTracePadding);
	FHitResult HitResult;

	FCollisionObjectQueryParams ObjParams(ECC_WorldStatic);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByObjectType(
		HitResult, 
		Start, 
		End, 
		ObjParams, 
		Params);
	return bHit;
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
		SetActorTickEnabled(false);
	}	
}

bool AItemBone::CanTakeItem(AActor* IgnoreActor)
{
	if (!BoneComponent) return false;

	FVector Start = BoneComponent->GetComponentLocation() + FVector(0.0f, 0.0f, TakeTraceStartOffset);
	FVector End = Start + FVector(0.0f, 0.0f, TakeTraceLength);

	FHitResult HitResult;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	if (IgnoreActor)
	{
		ActorsToIgnore.Add(IgnoreActor);
	}
	

	bool bHit = UKismetSystemLibrary::LineTraceSingle(
		this,
		Start,
		End,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		ActorsToIgnore,
		BoneDebugTraceMode.GetValue(),
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
	);

	return !(bHit && HitResult.IsValidBlockingHit());
}
void AItemBone::DisablePhysics()
{
	if (BoneComponent)
	{
		BoneComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
void AItemBone::EnablePhysics()
{
	if (BoneComponent)
	{
		BoneComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		VelocityZ = 0.0f;

		SetActorTickEnabled(true);
	}
}

// Called when the game starts or when spawned
void AItemBone::BeginPlay()
{
	Super::BeginPlay();

	FVector StartLocation = GetActorLocation();
}

// Called every frame
void AItemBone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePhysics(DeltaTime);

	float DeltaZ = VelocityZ * DeltaTime;
	
	FVector DeltaLocation = FVector(0.0f, 0.0f, DeltaZ);
	AddActorWorldOffset(DeltaLocation, true);
}

