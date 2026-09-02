// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveBox.h"
#include "DogPawn.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AInteractiveBox::AInteractiveBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	Player = nullptr;
	bIsFollowing = false;
	IsBoxBlocked = false;
	LastMoving = false;
	OriginalY = 0.0f;
	InterSpeed = 5.0f;

	BoxVelocityX = 0.0f;
	BoxVelocityZ = 0.0f;
	GravityScale = 2000.0f;
}

bool AInteractiveBox::IsOnGround()
{
	if (!BoxComponent) return false;

	FVector Start = BoxComponent->GetComponentLocation();

	float HalfHeight = BoxComponent->Bounds.BoxExtent.Z;
	FVector End = Start - FVector(0.0f, 0.0f, HalfHeight + 1.0f);

	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	if (Player) ActorsToIgnore.Add(Player);

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

float AInteractiveBox::GetBoxEdgeY() const
{
	if (!BoxComponent) return GetActorLocation().Y;

	float BoxCenter = BoxComponent->GetComponentLocation().Y;
	float BoxExtent = BoxComponent->Bounds.BoxExtent.Y;

	return BoxCenter + BoxExtent;
}
float AInteractiveBox::GetBoxLeftEdgeX() const
{
	if (!BoxComponent) return GetActorLocation().X;

	float BoxCenter = BoxComponent->GetComponentLocation().X;
	float BoxExtent = BoxComponent->Bounds.BoxExtent.X;

	return BoxCenter - BoxExtent;
}
float AInteractiveBox::GetBoxRightEdgeX() const
{
	if (!BoxComponent) return GetActorLocation().X;

	float BoxCenter = BoxComponent->GetComponentLocation().X;
	float BoxExtent = BoxComponent->Bounds.BoxExtent.X;

	return BoxCenter + BoxExtent;
}
float AInteractiveBox::GetBoxEdgeZ() const
{
	if (!BoxComponent) return GetActorLocation().Z;

	float BoxCenter = BoxComponent->GetComponentLocation().Z;
	float BoxExtent = BoxComponent->Bounds.BoxExtent.Z;

	return BoxCenter + BoxExtent;
}

void AInteractiveBox::UpdatePhysics(float DeltaTime)
{
	if (!BoxComponent) return;

	if (IsOnGround())
	{
		BoxVelocityZ = 0.0f;
	}
	else
	{
		BoxVelocityZ -= GravityScale * DeltaTime;

		if (bIsFollowing && BoxVelocityZ < -300.0f && Player)
		{
			Player->InteractReleased();
		}
	}
}
void AInteractiveBox::UpdateMovementX()
{
	if (!BoxComponent || !Player)
	{
		BoxVelocityX = 0.0f;
		return;
	}

	if (bIsFollowing)
	{
		BoxVelocityX = Player->GetVelocityX();
		return;
	}

	BoxVelocityX = 0.0f;

}

void AInteractiveBox::CheckWallCollision(const FHitResult& Hit)
{
	if (!Player || !bIsFollowing)
	{
		IsBoxBlocked = false;
		return;
	}

	if (IsBoxBlocked && Player->GetIsPulling())
	{
			IsBoxBlocked = false;
			return;
	}

	if (FMath::IsNearlyZero(Player->GetVelocityX(), 0.1f)) return;

	if (Hit.IsValidBlockingHit() && 
		!FMath::IsNearlyZero(Hit.ImpactNormal.X, 0.1f))
	{
		if ((BoxVelocityX > 0.0f && Hit.Normal.X < 0.0f) || 
			(BoxVelocityX < 0.0f && Hit.Normal.X > 0.0f))
		{
			IsBoxBlocked = true;
			Player->ForceStopMovement();
			return;
		}
	}
	IsBoxBlocked = false;
}

void AInteractiveBox::StartFollow(ADogPawn* NewPlayer)
{
	if (!NewPlayer) return;
	
	Player = NewPlayer;
	bIsFollowing = true;
	SetActorTickEnabled(true);

	BoxVelocityX = 0.0f;

	if (BoxComponent)
	{
		BoxComponent->IgnoreActorWhenMoving(Player, true);
	}
}
void AInteractiveBox::StopFollow()
{
	bIsFollowing = false;

	if (BoxComponent && Player)
	{
		BoxComponent->IgnoreActorWhenMoving(Player, false);
	}

	if (Player)
	{
		Player->ClearInteractiveBox();
		Player = nullptr;
	}
}

// Called when the game starts or when spawned
void AInteractiveBox::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent = Cast<UPrimitiveComponent>(GetRootComponent());

	OriginalY = GetActorLocation().Y;
}



// Called every frame
void AInteractiveBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdatePhysics(DeltaTime);
	UpdateMovementX();

	float DeltaX = BoxVelocityX * DeltaTime;
	float DeltaZ = BoxVelocityZ * DeltaTime;

	FHitResult HitResult;

	AddActorWorldOffset(FVector(DeltaX, 0.0f, DeltaZ), true, &HitResult);

	CheckWallCollision(HitResult);
	
}

