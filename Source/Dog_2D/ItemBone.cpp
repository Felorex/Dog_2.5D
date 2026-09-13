// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBone.h"
#include "DogPawn.h"
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

	Player = nullptr;
	BoneComponent = nullptr;

	IsTaking = false;
	IsFalling = false;
}

bool AItemBone::IsOnGround()
{
	if (!BoneComponent) return false;

	FVector Start = BoneComponent->GetComponentLocation();
	
	FVector Origin;
	FVector BoxExtent;
	GetActorBounds(false, Origin, BoxExtent);
	float HalfHeight = BoxExtent.Z;

	FVector End = Start - FVector(0.0f, 0.0f, HalfHeight + 2.f);
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
		SetActorTickEnabled(false);
	}	
}

bool AItemBone::TryTake(ADogPawn* NewPlayer)
{
	if (!BoneComponent|| !NewPlayer) return false;

	FVector Start = BoneComponent->GetComponentLocation() + FVector(0.0f, 0.0f, 20.f);
	FVector End = Start + FVector(0.0f, 0.0f, 60.f);

	FHitResult HitResult;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(NewPlayer);
	

	bool bHit = UKismetSystemLibrary::LineTraceSingle(
		this,
		Start,
		End,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
	);

	if (bHit && HitResult.IsValidBlockingHit())
	{
		NewPlayer->ClearItemBone();
		
		return false;
	}
	return true;
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
	
	BoneComponent = Cast<UPrimitiveComponent>(GetRootComponent());

}

// Called every frame
void AItemBone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePhysics(DeltaTime);

	float DeltaZ = VelocityZ * DeltaTime;

	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, FString::Printf(TEXT("VelocityZ: %f, CurrentZ: %f"), VelocityZ, GetActorLocation().Z));
	
	FVector DeltaLocation = FVector(0.0f, 0.0f, DeltaZ);
	AddActorWorldOffset(DeltaLocation, true);

	TArray<UPrimitiveComponent*> Prims;
	GetComponents<UPrimitiveComponent>(Prims);

	for (UPrimitiveComponent* Prim : Prims)
	{
		ECollisionEnabled::Type CollisionType = Prim->GetCollisionEnabled();
		ECollisionChannel CollisionChannel = Prim->GetCollisionObjectType();
		ECollisionResponse CollisionResponse = Prim->GetCollisionResponseToChannel(ECC_GameTraceChannel2);

		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, FString::Printf(TEXT("Component: %s, CollisionType: %d, CollisionChannel: %d, CollisionResponse: %s"),
			*Prim->GetName(),
			(int32)CollisionType,
			(int32)CollisionChannel,
			*UEnum::GetValueAsString(CollisionResponse)));
	}
}

