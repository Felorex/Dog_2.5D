// Fill out your copyright notice in the Description page of Project Settings.


#include "DogPawn.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ADogPawn::ADogPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bWantToCrouch = false;
	IsCrouching = false;

	bWantToJump = false;
	IsJumping = false;

	bIsInteracting = false;
	bIsPushing = false;
	bIsPulling = false;

	IsGrounded = false;	
	
	InteractDistance = 110.f;
	Gravity = -980.0f; 
	VelocityZ = 0.0f;
	VelocityX = 0.0f;
	MoveSpeed = 300.f;
	CrouchSpeed = 150.f;
	JumpForce = 600.0f;

	Box = nullptr;
	CollisionBody = nullptr;
	CollisionHead = nullptr;
	OriginalExtentBodyZ = 0.f;
	OriginalExtentHeadZ = 0.f;
	CrouchedScaleBodyZ = 0.6f;
	CrouchedScaleHeadZ = 0.8f;
}

void ADogPawn::UpdatePhysics(float DeltaTime)
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
void ADogPawn::Move(float Value)
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
	

	FVector DeltaLocation(DeltaX, 0.0f, 0.0f);
	FHitResult MoveResult;

	AddActorWorldOffset(DeltaLocation, true, &MoveResult);

	if (MoveResult.IsValidBlockingHit())
	{
		DeltaX = 0.f;
		ForceStopMovement();
	}
}
void ADogPawn::StopMove()
{
	VelocityX = 0.0f;

	SetMoveDirection(0.0f);
}
bool ADogPawn::CanMoveWithHead(float DeltaX)
{
	if (bIsInteracting && Box && bIsPushing && Box->GetIsBoxBlocked()) return false;

	if (bIsInteracting && Box && bIsPulling && Box->GetIsBoxBlocked()) return true;

	FVector Start = CollisionHead->GetComponentLocation();
	FVector HeadExtent = CollisionHead->GetScaledBoxExtent();
	FRotator Rotation = CollisionHead->GetComponentRotation();

	FVector End = Start;
	End.X += DeltaX;

	FHitResult HitResult;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	if (bIsInteracting && Box)
	{
		ActorsToIgnore.Add(Box);
	}

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
void ADogPawn::SetMoveDirection(float Value)
{
	if (!bIsInteracting || !Box)
	{
		if (bIsPulling || bIsPushing)
		{
			bIsPushing = false;
			bIsPulling = false;
			OnStopInteractVisual();
		}
		return;
	}

	bool WasPushing = bIsPushing;
	bool WasPulling = bIsPulling;

	float LookDirection = FMath::Sign(Conteiner->GetForwardVector().X);
	float InputDirection = FMath::Sign(Value);

	float RelativeDirection = LookDirection * InputDirection;

	if (RelativeDirection > 0.0f)
	{
		if (!bIsPushing)
		{
			bIsPushing = true;
			bIsPulling = false;
			OnPushVisual();
		}		
	}
	else
	{
		if (!bIsPulling)
		{
			bIsPushing = false;
			bIsPulling = true;
			OnPullVisual();
		}		
	}

	if (bIsPushing != WasPushing || bIsPulling != WasPulling)
	{
		InteractMovementX();
	}
}
void ADogPawn::CheckGrounded()
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
	if(IsJumping && VelocityZ > 0.0f)
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

void ADogPawn::DoJump()
{
	if (IsJumping || bIsInteracting || IsCrouching) return;

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
			&ADogPawn::CheckBoxUnderfoot,
			0.03,
			true
		);
	}
}
void ADogPawn::StopJump()
{
	bWantToJump = false;
}

void ADogPawn::CheckBoxUnderfoot()
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
		//EDrawDebugTrace::None,
		EDrawDebugTrace::ForDuration,
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
void ADogPawn::UpdatePositionY(float DeltaTime)
{
	if (!CollisionHead || !CollisionBody) return;

	if ((IsJumping || (IsGrounded && !bIsInteracting)) && Box)
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
	else if (bIsInteracting && Box)
	{
		float BoxY = Box->GetBoxEdgeY();
		float DogY = CollisionHead->GetUnscaledBoxExtent().Y;
		
		TargetY = DogY + BoxY;
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
void ADogPawn::CheckJumpExecution()
{
	if (!IsJumping) return;

	float CurrentY = GetActorLocation().Y;

	if (IsGrounded)
	{
		if (GetWorldTimerManager().IsTimerActive(CheckBoxTimer))
		{
			GetWorldTimerManager().ClearTimer(CheckBoxTimer);
		}
		
		if(!Box)
		{
			Box = nullptr;
		}

		IsJumping = false;
	}
}

void ADogPawn::CanInteractWithBox()
{
	if (bIsInteracting || IsCrouching || IsJumping) return;

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
	FVector BoxHalfExtent = FVector(30.0f, 120.0f, 40.0f);

	FHitResult Result;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	bool bHitSomthing = UKismetSystemLibrary::BoxTraceSingle(
		this,
		StartLocation,
		EndLocation,
		BoxHalfExtent,
		TraceRotation,
		UEngineTypes::ConvertToTraceType(ECC_WorldDynamic),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		//EDrawDebugTrace::ForDuration,
		Result,
		true
	);

	if (bHitSomthing && Result.GetActor())
	{
		AInteractiveBox* FoundBox = Cast<AInteractiveBox>(Result.GetActor());

		if (FoundBox)
		{
			Box = FoundBox;
			return;
		}
	}
	if (!bIsInteracting)
	{
		Box = nullptr;
	}
}
void ADogPawn::InteractPressed()
{
	if (Box)
	{
		if (GetActorLocation().Z > Box->GetBoxEdgeZ()) return;

		if (!PreCheckAlignmentSpace()) return;

		bIsInteracting = true;
		CollisionHead->IgnoreActorWhenMoving(Box, true);
		Box->StartFollow(this);

		InteractMovementX();
	}
}
void ADogPawn::InteractReleased()
{
	if (Box)
	{
		bIsInteracting = false;
		CollisionHead->IgnoreActorWhenMoving(Box, false);
		Box->StopFollow();
		OnStopInteractVisual();
	}
}
float ADogPawn::CalculateAlignmentDeltaX() const
{
	if (!Box || !CollisionHead) return 0.f;

	float DogDirection = FMath::Sign(Conteiner->GetForwardVector().X);

	float BoxWall = (DogDirection > 0.f) ? Box->GetBoxLeftEdgeX() :
		Box->GetBoxRightEdgeX();

	return BoxWall - GetHeadEdgeX();
}
bool ADogPawn::PreCheckAlignmentSpace()
{
	if (!Box || !CollisionHead) return true;

	float DeltaX = CalculateAlignmentDeltaX();

	if (FMath::IsNearlyEqual(DeltaX, 1.f)) return true;

	FHitResult HitResult;

	AddActorWorldOffset(FVector(DeltaX, 0.f, 0.f), true, &HitResult);

	if (HitResult.IsValidBlockingHit())
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor->IsA(AInteractiveBox::StaticClass()) == false)
		{
			float RealMovedDistance = DeltaX * HitResult.Time;
			AddActorWorldOffset(FVector(-RealMovedDistance, 0.f, 0.f), false);
			return false;
		}
		
	}
	AddActorWorldOffset(FVector(-DeltaX, 0.f, 0.f), false);
	return true;
}
void ADogPawn::InteractMovementX()
{
	if (!bIsInteracting || !Box || !CollisionHead) return;
	
	float DeltaX = CalculateAlignmentDeltaX();

	AddActorWorldOffset(FVector(DeltaX, 0.f, 0.f), false);

}
void ADogPawn::ForceStopMovement()
{
	VelocityX = 0.0f;
	SetMoveDirection(0.0f);
}
void ADogPawn::ClearInteractiveBox()
{
	Box = nullptr;
}

void ADogPawn::OnCrouchPressed()
{
	if (IsCrouching || IsJumping || bIsInteracting) return;

	bWantToCrouch = true;

	if(IsGrounded)
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
void ADogPawn::OnCrouchReleased()
{
	bWantToCrouch = false;

	if (IsCrouching)
	{
		TryStandUp();
	}
}
void ADogPawn::TryStandUp()
{
	if (CanStandUp())
	{
		IsCrouching = false;

		OnStandVisual();
	}	
}
bool ADogPawn::CanStandUp()
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

void ADogPawn::Depenetration()
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
void ADogPawn::DepenetrationZ()
{
	if (IsGrounded && FMath::IsNearlyZero(VelocityZ, 0.1f)) return;

	if (!CollisionBody) return;

	FVector BodyLoc = CollisionBody->GetComponentLocation();
	FVector BodyExtent = CollisionBody->GetScaledBoxExtent();
	FQuat Rotation = CollisionBody->GetComponentQuat();
	FHitResult HitResult;

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	bool bOverlap = GetWorld()->SweepSingleByObjectType(
		HitResult,
		BodyLoc,
		BodyLoc,
		Rotation,
		ObjectParams,
		FCollisionShape::MakeBox(BodyExtent),
		FCollisionQueryParams::DefaultQueryParam
	);

	if (bOverlap && HitResult.bStartPenetrating)
	{
		FVector DepenetrationVector = HitResult.Normal * (HitResult.PenetrationDepth + 0.5f);

		DepenetrationVector.X = 0.f;
		DepenetrationVector.Y = 0.f;

		AddActorWorldOffset(DepenetrationVector, false);
	}
}

float ADogPawn::GetHeadEdgeX() const
{
	if (!Conteiner || !CollisionHead) return GetActorLocation().X;

	float Direction = FMath::Sign(Conteiner->GetForwardVector().X);
	return CollisionHead->GetComponentLocation().X + (CollisionHead->GetScaledBoxExtent().X * Direction);
}
float ADogPawn::GetHeadEdgeY() const
{
	if (!Conteiner || !CollisionHead) return GetActorLocation().Y;

	float HeadCenterY = CollisionHead->GetComponentLocation().Y;
	float HeadExtentY = CollisionHead->GetScaledBoxExtent().Y;

	return HeadCenterY - HeadExtentY;
}
float ADogPawn::GetHeadEdgeZ() const
{
	if (!CollisionHead) return GetActorLocation().Z;

	float HeadCenterZ = CollisionHead->GetComponentLocation().Y;
	float HeadExtentZ = CollisionHead->GetScaledBoxExtent().Z;

	return HeadCenterZ + HeadExtentZ;
}
float ADogPawn::GetBottomZ() const
{
	if (!CollisionBody) return GetActorLocation().Z;

	return GetActorLocation().Z - CollisionBody->GetScaledBoxExtent().Z;
}


// Called when the game starts or when spawned
void ADogPawn::BeginPlay()
{
	Super::BeginPlay();

	Conteiner = Cast<USceneComponent>(GetDefaultSubobjectByName(TEXT("VisualConteiner")));
	CameraComp = Cast<USceneComponent>(GetDefaultSubobjectByName(TEXT("SpringArm")));
	CollisionBody = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("BodyCollision")));
	CollisionHead = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("HeadCollision")));

	OriginalExtentBodyZ = CollisionBody->GetUnscaledBoxExtent().Z;
	OriginalExtentHeadZ = CollisionHead->GetUnscaledBoxExtent().Z;

	OriginalY = GetActorLocation().Y;
	TargetY = OriginalY;

	if (CollisionBody && CollisionHead)
	{
		CollisionBody->IgnoreComponentWhenMoving(CollisionHead, true);
		CollisionHead->IgnoreComponentWhenMoving(CollisionBody, true);
	}
	

	GetWorldTimerManager().SetTimer(
		InteractTimer,
		this,
		&ADogPawn::CanInteractWithBox,
		0.1f,
		true
	);
		
}

// Called every frame
void ADogPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckGrounded();
	UpdatePhysics(DeltaTime);
	UpdatePositionY(DeltaTime);

	Depenetration();

	if (CameraComp)
	{
		FVector CameraLoc = CameraComp->GetComponentLocation();
		CameraLoc.Y = OriginalY;
		CameraComp->SetWorldLocation(CameraLoc);
	}

	if (IsJumping)
	{
		CheckJumpExecution();
	}
	
	if (!bWantToCrouch && IsCrouching)
	{
		TryStandUp();
	}

	
}

// Called to bind functionality to input
void ADogPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

