// Fill out your copyright notice in the Description page of Project Settings.


#include "DogPawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InteractiveBox.h"

// Sets default values
ADogPawn::ADogPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsInteracting = false;
	bIsPushing = false;
	bIsPulling = false;
}

void ADogPawn::Move(float Value)
{
	Super::Move(Value);

	if (!bIsInteracting && !FMath::IsNearlyZero(Value, 0.1f))
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
	else
	{
		SetMoveDirection(Value);
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


void ADogPawn::DoJump() 
{
	if (bIsInteracting) return;

	Super::DoJump();
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

void ADogPawn::CanInteractWithObjects()
{
	if (bIsInteracting) return;

	Super::CanInteractWithObjects();

	if (InteractHitResult.IsValidBlockingHit() && InteractHitResult.GetActor())
	{
		AInteractiveBox* FoundBox = Cast<AInteractiveBox>(InteractHitResult.GetActor());

		if (FoundBox)
		{
			Box = FoundBox;
			Bone = nullptr;
			return;
		}

		if (Bone)
		{
			Box = nullptr;
			return;
		}
	}
	else 
	{
		if (!bIsInteracting) Box = nullptr;

		if (!bIsTakingItem) Bone = nullptr;
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

void ADogPawn::OnCrouchPressed()
{
	if (bIsInteracting) return;

	Super::OnCrouchPressed();
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

float ADogPawn::GetContainerForward() const
{
	if (!Conteiner) return 1.f;
	return FMath::Sign(Conteiner->GetForwardVector().X);
}


// Called when the game starts or when spawned
void ADogPawn::BeginPlay()
{
	Super::BeginPlay();

	CameraComp = Cast<USceneComponent>(GetDefaultSubobjectByName(TEXT("SpringArm")));		
}

// Called every frame
void ADogPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

