// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseDogPawn.h"
#include "Interactive/InteractiveBox.h"
#include "Interactive/ItemBone.h"

// Sets default values
ABaseDogPawn::ABaseDogPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBody = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBody"));
	if (CollisionBody)
	{
		RootComponent = CollisionBody;
	}

	VisualContainer = CreateDefaultSubobject<USceneComponent>(TEXT("VisualContainer"));
	if (CollisionBody && VisualContainer)
	{
		VisualContainer->SetupAttachment(CollisionBody);
	}

	CollisionHead = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionHead"));
	if (VisualContainer && CollisionHead)
	{
		CollisionHead->SetupAttachment(VisualContainer);
	}

	Head = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Head"));
	if (CollisionHead && Head)
	{
		Head->SetupAttachment(CollisionHead);
	}
	
	MouthComp = CreateDefaultSubobject<USceneComponent>(TEXT("MouthAttachPoint"));
	if (MouthComp && Head)
	{
		MouthComp->SetupAttachment(Head);
	}

	VelocityZ = 0.0f;
	VelocityX = 0.0f;

	bIsGrounded = false;
	bIsCrouching = false;
	bIsJumping = false;
	bWantToCrouch = false;
	bWantToJump = false;
	bIsTakingItem = false;

	Box = nullptr;
	Bone = nullptr;
}

void ABaseDogPawn::Move(float Value)
{
	float CurrentMoveSpeed = bIsCrouching ? CrouchSpeed : MoveSpeed;
	VelocityX = Value * CurrentMoveSpeed;

	float DeltaX = VelocityX * GetWorld()->GetDeltaSeconds();

	if (!CanMoveWithHead(FVector(DeltaX, 0.f, 0.f)))
	{
		ForceStopMovement();
		DeltaX = 0.0f;
		return;
	}

	UpdateMovementVisual(Value);

	FVector DeltaLocation(DeltaX, 0.0f, 0.0f);
	FHitResult MoveResult;

	AddActorWorldOffset(DeltaLocation, true, &MoveResult);

	if (MoveResult.IsValidBlockingHit())
	{
		DeltaX = 0.f;
		ForceStopMovement();
	}
}
void ABaseDogPawn::UpdateMovementVisual(float Value)
{
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
}
bool ABaseDogPawn::CanMoveWithHead(const FVector& Delta)
{
	if (!CollisionHead) return false;

	FVector Start = CollisionHead->GetComponentLocation();
	FVector HeadExtent = CollisionHead->GetScaledBoxExtent();
	FRotator Rotation = CollisionHead->GetComponentRotation();

	FVector End = Start + Delta;

	FHitResult HitResult;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	GetAdditionalActorsToIgnore(ActorsToIgnore);

	bool bHit = UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		HeadExtent,
		Rotation,
		UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
		false,
		ActorsToIgnore,
		DogDebugTraceMode.GetValue(),
		HitResult,
		true
	);

	return !(bHit && HitResult.IsValidBlockingHit());
}
void ABaseDogPawn::ForceStopMovement()
{
	VelocityX = 0.0f;
}

void ABaseDogPawn::DoJump()
{
	if (bIsJumping || bIsCrouching) return;

	if (Box)
	{
		ClearInteractiveBox();
	}
	if (bIsGrounded)
	{
		VelocityZ = JumpForce;
		bIsGrounded = false;
		bIsJumping = true;
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
	if (!bIsGrounded)
	{
		VelocityZ += Gravity * DeltaTime;
	}
	else
	{
		VelocityZ = 0.0f;
	}

	float DeltaZ = VelocityZ * DeltaTime;

	if (!FMath::IsNearlyZero(DeltaZ) && !CanMoveWithHead(FVector(0.f, 0.f, DeltaZ)))
	{
		VelocityZ = 0.f;
		DeltaZ = 0.f;
	}

	FVector DeltaLocation(0.0f, 0.0f, DeltaZ);
	AddActorWorldOffset(DeltaLocation, true);
}

void ABaseDogPawn::OnCrouchPressed()
{
	if (bIsCrouching || bIsJumping) return;

	bWantToCrouch = true;

	if (bIsGrounded)
	{
		bIsCrouching = true;

		if (Box)
		{
			float BoxTop = Box->GetBoxEdgeZ();
			float DogBottom = GetBottomZ();

			if (DogBottom < BoxTop - BoxDropThreshold)
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

	if (bIsCrouching)
	{
		TryStandUp();
	}
}
void ABaseDogPawn::TryStandUp()
{
	if (CanStandUp())
	{
		bIsCrouching = false;

		OnStandVisual();
	}
}
bool ABaseDogPawn::CanStandUp()
{
	if (!bIsCrouching || !CollisionBody || !CollisionHead) return true;

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
		DogDebugTraceMode.GetValue(),
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
		DogDebugTraceMode.GetValue(),
		HitBodyResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
	);

	return !(bHitBody || bHitHead);
}

bool ABaseDogPawn::ScanForInteractableObject(FHitResult& HitResult)
{
	FVector LookDirection = GetActorForwardVector();
	FRotator TraceRotation = GetActorRotation();

	if (VisualContainer)
	{
		LookDirection = VisualContainer->GetForwardVector();
		TraceRotation = VisualContainer->GetComponentRotation();
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

	StartLocation.Z += InteractTraceStartHeightOffset;
	EndLocation.Z += InteractTraceEndHeightOffset;

	ETraceTypeQuery InteractTraceChannel = UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	return UKismetSystemLibrary::BoxTraceSingle(
		this,
		StartLocation,
		EndLocation,
		InteractBoxExtent,
		TraceRotation,
		InteractTraceChannel,
		false,
		ActorsToIgnore,
		DogDebugTraceMode.GetValue(),
		HitResult,
		true
	);
}
void ABaseDogPawn::CanInteractWithObjects()
{
	if (!CollisionBody || !CollisionHead) return;

	if (bIsCrouching || bIsJumping || bIsTakingItem) return;

	if (!bIsTakingItem) Bone = nullptr;

	bool bHitSomething = ScanForInteractableObject(InteractHitResult);

	if (bHitSomething && InteractHitResult.GetActor())
	{
		AItemBone* FoundBone = Cast<AItemBone>(InteractHitResult.GetActor());

		if (FoundBone)
		{
			Bone = FoundBone;
			return;
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

	return (Bone->CanTakeItem(this));
}
void ABaseDogPawn::AttachItemToMouth()
{
	if (!Bone || !MouthComp || bIsTakingItem) return;

	Bone->DisablePhysics();

	FAttachmentTransformRules AttachRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepRelative,
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

float ABaseDogPawn::CalculateTargetY() const
{
	if ((bIsGrounded || bIsJumping) && Box)
	{
		float BoxTop = Box->GetBoxEdgeZ();
		float DogBottom = GetBottomZ();

		if ((DogBottom + BoxAlignmentZOffset) > BoxTop)
		{
			return Box->GetActorLocation().Y;
		}
	}
	return OriginalY;
}
void ABaseDogPawn::ApplyPositionIntrolationY(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();
	float NewY = FMath::FInterpTo(CurrentLocation.Y, TargetY, DeltaTime, YInterpSpeed);
	float DeltaY = NewY - CurrentLocation.Y;

	AddActorWorldOffset(FVector(0.f, DeltaY, 0.f), false);
}
void ABaseDogPawn::UpdatePositionY(float DeltaTime)
{
	if (!CollisionHead || !CollisionBody) return;

	TargetY = CalculateTargetY();

	ApplyPositionIntrolationY(DeltaTime);
}

float ABaseDogPawn::GetBottomZ() const
{
	if (!CollisionBody) return GetActorLocation().Z;

	return GetActorLocation().Z - CollisionBody->GetScaledBoxExtent().Z;
}

float ABaseDogPawn::GetMinCollisionX() const
{
	if (!CollisionBody || !CollisionHead) return GetActorLocation().X;

	float BodyLeft = CollisionBody->GetComponentLocation().X - CollisionBody->GetScaledBoxExtent().X;
	float HeadLeft = CollisionHead->GetComponentLocation().X - CollisionHead->GetScaledBoxExtent().X;

	return FMath::Min(BodyLeft, HeadLeft);
}
float ABaseDogPawn::GetMaxCollisionX() const
{
	if (!CollisionBody || !CollisionHead) return GetActorLocation().X;

	float BodyRight = CollisionBody->GetComponentLocation().X + CollisionBody->GetScaledBoxExtent().X;
	float HeadRight = CollisionHead->GetComponentLocation().X + CollisionHead->GetScaledBoxExtent().X;

	return FMath::Max(BodyRight, HeadRight);
}

void ABaseDogPawn::ClearInteractiveBox()
{
	Box = nullptr;
}

bool ABaseDogPawn::IsTouchingGround(FHitResult& OutHitResult)
{
	if (!CollisionBody) return false;

	FVector Start = CollisionBody->GetComponentLocation();
	FVector BodyExtent = CollisionBody->GetScaledBoxExtent();
	FRotator Rotation = CollisionBody->GetComponentRotation();

	FVector End = Start;
	End.Z -= GroundCheckDistance;

	FVector TraceExtent = FVector(GroundTraceHalfWidth, BodyExtent.Y, BodyExtent.Z);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	return UKismetSystemLibrary::BoxTraceSingleForObjects(
		this,
		Start,
		End,
		TraceExtent,
		Rotation,
		ObjectTypes,
		false,
		TArray<AActor*>({ this }),
		DogDebugTraceMode.GetValue(),
		OutHitResult,
		true
	);
}
void ABaseDogPawn::AnalyzeGroundType(AActor* HitActor)
{
	if (!HitActor) return;

	if (HitActor->IsA(AInteractiveBox::StaticClass()))
	{
		Box = Cast<AInteractiveBox>(HitActor);
	}
}
void ABaseDogPawn::CheckGrounded()
{
	FHitResult HitResult;

	bool bHasPhysicalContact = IsTouchingGround(HitResult);

	if (bIsJumping && VelocityZ > 0.0f)
	{
		bIsGrounded = false;
		return;
	}
	if (bHasPhysicalContact && HitResult.IsValidBlockingHit())
	{
		bIsGrounded = true;

		AnalyzeGroundType(HitResult.GetActor());
	}
	else
	{
		bIsGrounded = false;
	}
}

bool ABaseDogPawn::ScanForBoxUnderfoot(FHitResult& HitResult)
{
	FVector Start = GetActorLocation();

	FVector End = Start - FVector(0.0f, 0.0f, BoxSearchDistance);

	TArray<AActor*> ActorToIgnore = { this };

	return UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		BoxSearchExtent,
		GetActorRotation(),
		UEngineTypes::ConvertToTraceType(ECC_WorldDynamic),
		false,
		ActorToIgnore,
		DogDebugTraceMode.GetValue(),
		HitResult,
		true
	);
}
void ABaseDogPawn::CheckBoxUnderfoot()
{
	if (!bIsJumping) return;

	FHitResult Result;
	bool bHitSomething = ScanForBoxUnderfoot(Result);

	if (bHitSomething && Result.GetActor())
	{
		AInteractiveBox* FoundBox = Cast<AInteractiveBox>(Result.GetActor());
		if (FoundBox)
		{
			float BoxLeft = FoundBox->GetBoxLeftEdgeX();
			float BoxRight = FoundBox->GetBoxRightEdgeX();

			float DogX = GetActorLocation().X;

			if (Result.ImpactNormal.Z > BoxMinSurfaceNormalZ && DogX > BoxLeft && DogX < BoxRight)
			{
				Box = FoundBox;
			}
		}
	}
}
void ABaseDogPawn::CheckJumpExecution()
{
	if (!bIsJumping) return;

	if (bIsGrounded)
	{
		if (GetWorldTimerManager().IsTimerActive(CheckBoxTimer))
		{
			GetWorldTimerManager().ClearTimer(CheckBoxTimer);
		}
		bIsJumping = false;
	}
}


void ABaseDogPawn::DepenetrationHead()
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
		FVector DepenetrationVector = HitResult.Normal * (HitResult.PenetrationDepth + DepenetrationPadding);

		DepenetrationVector.Z = 0.0f;
		DepenetrationVector.Y = 0.0f;

		AddActorWorldOffset(DepenetrationVector, false);
	}
}

// Called when the game starts or when spawned
void ABaseDogPawn::BeginPlay()
{
	Super::BeginPlay();
	
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
		InteractionTimerInterval,
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
	DepenetrationHead();

	if (bIsJumping)
	{
		CheckJumpExecution();
	}

	if (!bWantToCrouch && bIsCrouching)
	{
		TryStandUp();
	}
}