// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDogPawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InteractiveBox.h"

// Sets default values
ABaseDogPawn::ABaseDogPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Gravity = -980.0f;
	VelocityZ = 0.0f;
	VelocityX = 0.0f;
	MoveSpeed = 300.0f;
	CrouchSpeed = 150.0f;
	JumpForce = 600.f;

	IsGrounded = false;
	IsCrouching = false;
	IsJumping = false;
	bWantToCrouch = false;
	bWantToJump = false;

	Box = nullptr;

	CollisionBody = nullptr;
	CollisionHead = nullptr;
	Conteiner = nullptr;
	MouthComp = nullptr;
}

void ABaseDogPawn::DoJump()
{
	if (IsJumping || /*bIsInteracting ||*/ IsCrouching) return;

	if (Box)
	{
		ClearInteractiveBox();
	}
	if (IsGrounded)
	{
		VelocityZ = JumpForce;
		IsGrounded = false;
		IsJumping = true;
		bWantToJump = true;

		GetWorldTimerManager().SetTimer(
			CheckBoxTimer,
			this,
			&ABaseDogPawn::CheckBoxUnderfoot,
			0.03f,
			true
		);
	}
}
void ABaseDogPawn::StopJump()
{
	bWantToJump = false;
}

void ABaseDogPawn::UpdatePhysics(float DeltaTime)
{
	if (!IsGrounded)
	{
		VelocityZ += Gravity * DeltaTime;
	}
	else
	{
		VelocityZ = 0.0f;
	}

	FVector DeltaLocation(0.0f, 0.0f, VelocityZ * DeltaTime);
	AddActorWorldOffset(DeltaLocation, true);
}
void ABaseDogPawn::UpdatePositionY(float DeltaTime)
{
	if (!CollisionHead || !CollisionBody) return;

	if (IsJumping && Box)
	{
		float BoxTop = Box->GetBoxEdgeZ();
		float DogBottom = GetBottomZ();

		if ((DogBottom + 5.f) > BoxTop)
		{
			TargetY = Box->GetActorLocation().Y;
		}
		else
		{
			TargetY = OriginalY;
		}
	}
	else
	{
		TargetY = OriginalY;
	}

	FVector CurrentLocation = GetActorLocation();
	float NewY = FMath::FInterpTo(CurrentLocation.Y, TargetY, DeltaTime, 5.f);
	float DeltaY = NewY - CurrentLocation.Y;
	AddActorWorldOffset(FVector(0.f, DeltaY, 0.f), false);
}

float ABaseDogPawn::GetBottomZ() const
{
	if (!CollisionBody) return GetActorLocation().Z;

	return GetActorLocation().Z - CollisionBody->GetScaledBoxExtent().Z;
}

void ABaseDogPawn::ClearInteractiveBox()
{
	Box = nullptr;
}

void ABaseDogPawn::CheckGrounded()
{
	if (!CollisionBody) return;

	FVector Start = CollisionBody->GetComponentLocation();
	FVector BodyExtent = CollisionBody->GetScaledBoxExtent();
	FRotator Rotation = CollisionBody->GetComponentRotation();

	FVector End = Start;
	End.Z -= 5.0f; // Check slightly below the body

	FVector TraceExtent = FVector(15.f, BodyExtent.Y, BodyExtent.Z);

	FHitResult HitResult;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	bool bHit = UKismetSystemLibrary::BoxTraceSingleForObjects(
		this,
		Start,
		End,
		TraceExtent,
		Rotation,
		ObjectTypes,
		false,
		TArray<AActor*>({ this }),
		EDrawDebugTrace::None,
		//EDrawDebugTrace::ForDuration,
		HitResult,
		true
	);
	if (IsJumping && VelocityZ > 0.0f)
	{
		IsGrounded = false;
		return;
	}
	if (bHit && HitResult.IsValidBlockingHit())
	{
		IsGrounded = true;

		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor->IsA(AInteractiveBox::StaticClass()))
		{
			Box = Cast<AInteractiveBox>(HitActor);
		}
	}
	else
	{
		IsGrounded = false;
	}
}

void ABaseDogPawn::CheckBoxUnderfoot()
{
	if (!IsJumping) return;

	FVector Start = GetActorLocation();

	FVector End = Start - FVector(0.0f, 0.0f, 120.f);
	FVector BoxHalfExtent = FVector(30.0f, 100.0f, 10.f);

	FHitResult Result;

	TArray<AActor*> ActorToIgnore = { this };

	bool bHit = UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		BoxHalfExtent,
		GetActorRotation(),
		UEngineTypes::ConvertToTraceType(ECC_WorldDynamic),
		false,
		ActorToIgnore,
		EDrawDebugTrace::None,
		//EDrawDebugTrace::ForDuration,
		Result,
		true
	);

	if (bHit && Result.GetActor())
	{
		AInteractiveBox* FoundBox = Cast<AInteractiveBox>(Result.GetActor());
		if (FoundBox)
		{
			float BoxLeft = FoundBox->GetBoxLeftEdgeX();
			float BoxRight = FoundBox->GetBoxRightEdgeX();

			float DogX = GetActorLocation().X;

			if (Result.ImpactNormal.Z > 0.7f && DogX > BoxLeft && DogX < BoxRight)
			{
				Box = FoundBox;
			}
		}
	}
}
void ABaseDogPawn::CheckJumpExecution()
{
	if (!IsJumping) return;

	if (IsGrounded)
	{
		if (GetWorldTimerManager().IsTimerActive(CheckBoxTimer))
		{
			GetWorldTimerManager().ClearTimer(CheckBoxTimer);
		}

		if (!Box)
		{
			Box = nullptr;
		}
		IsJumping = false;
	}
}


void ABaseDogPawn::Depenetration()
{
	if (!CollisionHead) return;

	FVector HeadLoc = CollisionHead->GetComponentLocation();
	FVector HeadExtent = CollisionHead->GetScaledBoxExtent();
	FQuat Rotation = CollisionHead->GetComponentQuat();
	FHitResult HitResult;

	bool bOverlap = GetWorld()->SweepSingleByChannel(
		HitResult,
		HeadLoc,
		HeadLoc,
		Rotation,
		ECC_WorldStatic,
		FCollisionShape::MakeBox(HeadExtent),
		FCollisionQueryParams::DefaultQueryParam
	);

	if (bOverlap && HitResult.bStartPenetrating)
	{
		FVector DepenetrationVector = HitResult.Normal * (HitResult.PenetrationDepth + 1.0f);

		DepenetrationVector.Z = 0.0f;
		DepenetrationVector.Y = 0.0f;

		AddActorWorldOffset(DepenetrationVector, false);
	}
}


// Called when the game starts or when spawned
void ABaseDogPawn::BeginPlay()
{
	Super::BeginPlay();
	
	Conteiner = Cast<USceneComponent>(GetDefaultSubobjectByName(TEXT("VisualConteiner")));
	MouthComp = Cast<USceneComponent>(GetDefaultSubobjectByName(TEXT("MouthAttachPoint")));
	CollisionBody = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("BodyCollision")));
	CollisionHead = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("HeadCollision")));

	OriginalY = GetActorLocation().Y;
	TargetY = OriginalY;

	if (CollisionBody && CollisionHead)
	{
		OriginalExtentBodyZ = CollisionBody->GetUnscaledBoxExtent().Z;
		OriginalExtentHeadZ = CollisionHead->GetUnscaledBoxExtent().Z;
		CollisionBody->IgnoreComponentWhenMoving(CollisionHead, true);
		CollisionHead->IgnoreComponentWhenMoving(CollisionBody, true);
	}
}

// Called every frame
void ABaseDogPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckGrounded();
	UpdatePhysics(DeltaTime);
	UpdatePositionY(DeltaTime);

	Depenetration();
}