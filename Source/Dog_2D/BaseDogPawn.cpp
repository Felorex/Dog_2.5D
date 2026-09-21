// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDogPawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InteractiveBox.h"
#include "ItemBone.h"

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

	bIsTakingItem = false;

	InteractDistance = 100.f;

	Box = nullptr;
	Bone = nullptr;

	CollisionBody = nullptr;
	CollisionHead = nullptr;
	Conteiner = nullptr;
	MouthComp = nullptr;
}

void ABaseDogPawn::Move(float Value)
{
	float CurrentMoveSpeed = IsCrouching ? CrouchSpeed : MoveSpeed;
	VelocityX = Value * CurrentMoveSpeed;

	float DeltaX = VelocityX * GetWorld()->GetDeltaSeconds();

	if (!CanMoveWithHead(DeltaX))
	{
		ForceStopMovement();
		DeltaX = 0.0f;
		return;
	}

	if (!FMath::IsNearlyZero(Value, 0.1f))
	{
		if (Value < 0.f)
		{
			OnLookLeftVisual();
		}
		else if (Value > 0.f)
		{
			OnLookRightVisual();
		}
	}

	FVector DeltaLocation(DeltaX, 0.0f, 0.0f);
	FHitResult MoveResult;

	AddActorWorldOffset(DeltaLocation, true, &MoveResult);

	if (MoveResult.IsValidBlockingHit())
	{
		DeltaX = 0.f;
		ForceStopMovement();
	}
}
bool ABaseDogPawn::CanMoveWithHead(float DeltaX)
{
	FVector Start = CollisionHead->GetComponentLocation();
	FVector HeadExtent = CollisionHead->GetScaledBoxExtent();
	FRotator Rotation = CollisionHead->GetComponentRotation();

	FVector End = Start;
	End.X += DeltaX;

	FHitResult HitResult;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	bool bHit = UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		HeadExtent,
		Rotation,
		UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		//EDrawDebugTrace::ForDuration,
		HitResult,
		true
	);

	if (bHit && HitResult.IsValidBlockingHit())
	{
		return false;
	}
	return true;
}
void ABaseDogPawn::ForceStopMovement()
{
	VelocityX = 0.0f;
}
void ABaseDogPawn::StopMove()
{
	VelocityX = 0.0f;
}

void ABaseDogPawn::DoJump()
{
	if (IsJumping || IsCrouching) return;

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

void ABaseDogPawn::OnCrouchPressed()
{
	if (IsCrouching || IsJumping) return;

	bWantToCrouch = true;

	if (IsGrounded)
	{
		IsCrouching = true;

		if (Box)
		{
			float BoxTop = Box->GetBoxEdgeZ();
			float DogBottom = GetBottomZ();

			if (DogBottom < BoxTop - 5.f)
			{
				ClearInteractiveBox();
			}
		}
		OnCrouchVisual();
	}
}
void ABaseDogPawn::OnCrouchReleased()
{
	bWantToCrouch = false;

	if (IsCrouching)
	{
		TryStandUp();
	}
}
void ABaseDogPawn::TryStandUp()
{
	if (CanStandUp())
	{
		IsCrouching = false;

		OnStandVisual();
	}
}
bool ABaseDogPawn::CanStandUp()
{
	if (!IsCrouching) return true;

	FVector HeadLocation = CollisionHead->GetComponentLocation();
	FVector HeadExtent = CollisionHead->GetScaledBoxExtent();
	FRotator Rotation = FRotator::ZeroRotator;
	HeadLocation.Z += (OriginalExtentHeadZ - HeadExtent.Z);

	FVector FullHeadExtent = CollisionHead->GetUnscaledBoxExtent();
	FullHeadExtent.Z = OriginalExtentHeadZ;

	FHitResult HitHeadResult;

	bool bHitHead = UKismetSystemLibrary::BoxTraceSingle(
		this,
		HeadLocation,
		HeadLocation,
		FullHeadExtent,
		Rotation,
		UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
		false,
		TArray<AActor*>({ this }),
		EDrawDebugTrace::ForDuration,
		HitHeadResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
	);

	FVector BodyLocation = CollisionBody->GetComponentLocation();
	FVector BodyExtent = CollisionBody->GetScaledBoxExtent();
	FRotator BodyRotation = CollisionBody->GetComponentRotation();
	BodyLocation.Z += (OriginalExtentBodyZ - BodyExtent.Z);

	FVector FullBodyExtent = CollisionBody->GetUnscaledBoxExtent();
	FullBodyExtent.Z = OriginalExtentBodyZ;

	FHitResult HitBodyResult;

	bool bHitBody = UKismetSystemLibrary::BoxTraceSingle(
		this,
		BodyLocation,
		BodyLocation,
		FullBodyExtent,
		BodyRotation,
		UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
		false,
		TArray<AActor*>({ this }),
		//EDrawDebugTrace::None,
		EDrawDebugTrace::ForDuration,
		HitBodyResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
	);

	if (bHitBody || bHitHead)
	{
		return false;
	}
	return true;
}

void ABaseDogPawn::CanInteractWithObjects()
{
	if (IsCrouching || IsJumping || bIsTakingItem) return;

	FVector LookDirection = GetActorForwardVector();
	FRotator TraceRotation = GetActorRotation();


	if (Conteiner)
	{
		LookDirection = Conteiner->GetForwardVector();
		TraceRotation = Conteiner->GetComponentRotation();
	}

	FVector StartLocation = GetActorLocation();

	if (CollisionBody)
	{
		float BodyCenter = CollisionBody->GetComponentLocation().X;
		float BodyHalf = CollisionBody->GetScaledBoxExtent().X;
		float DogDirecion = FMath::Sign(LookDirection.X);

		StartLocation.X = BodyCenter + (BodyHalf * DogDirecion);

	}

	FVector EndLocation = StartLocation + (LookDirection * InteractDistance);

	StartLocation.Z += 5.f;
	EndLocation.Z += 2.f;

	FVector BoxHalfExtent = FVector(30.0f, 120.0f, 65.f);

	ETraceTypeQuery InteractTraceChannel = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	InteractHitResult = FHitResult();

	bool bHitSomthing = UKismetSystemLibrary::BoxTraceSingle(
		this,
		StartLocation,
		EndLocation,
		BoxHalfExtent,
		TraceRotation,
		InteractTraceChannel,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		//EDrawDebugTrace::ForDuration,
		InteractHitResult,
		true
	);

	if (bHitSomthing && InteractHitResult.GetActor())
	{
		AItemBone* FoundBone = Cast<AItemBone>(InteractHitResult.GetActor());

		if (FoundBone)
		{
			Bone = FoundBone;
			return;
		}
	}
	else
	{
		if (!bIsTakingItem)
		{
			Bone = nullptr;
		}
	}
}

void ABaseDogPawn::TakeItemPressed()
{
	if (!Bone) return;

	if (!bIsTakingItem)
	{
		if (WantToTakeItem())
		{
			OnPickupVisual();
		}
	}
	else
	{
		OnDropVisual();
	}
}
bool ABaseDogPawn::WantToTakeItem()
{
	if (!Bone) return false;

	if (Bone->TryTake(this))
	{
		return true;
	}
	return false;
}
void ABaseDogPawn::AttachItemToMouth()
{
	if (!Bone || !MouthComp || bIsTakingItem) return;

	Bone->DisablePhysics();

	FAttachmentTransformRules AttachRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepWorld,
		false
	);
	Bone->AttachToComponent(MouthComp, AttachRules);

	bIsTakingItem = true;
}
void ABaseDogPawn::DetachItemFromMouth()
{
	if (!Bone || !MouthComp || !bIsTakingItem) return;

	Bone->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	Bone->EnablePhysics();
	ClearItemBone();
}
void ABaseDogPawn::ClearItemBone()
{
	Bone = nullptr;
	bIsTakingItem = false;
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

	GetWorldTimerManager().SetTimer(
		InteractTimer,
		this,
		&ABaseDogPawn::CanInteractWithObjects,
		0.1f,
		true
	);
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