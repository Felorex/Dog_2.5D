// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDogPawn.h"

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

	CollisionBody = nullptr;
	CollisionHead = nullptr;
	Conteiner = nullptr;
	MouthComp = nullptr;
}

// Called when the game starts or when spawned
void ABaseDogPawn::BeginPlay()
{
	Super::BeginPlay();
	
	Conteiner = Cast<USceneComponent>(GetDefaultSubobjectByName(TEXT("VisualConteiner")));
	MouthComp = Cast<USceneComponent>(GetDefaultSubobjectByName(TEXT("MouthAttachPoint")));
	CollisionBody = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("BodyCollision")));
	CollisionHead = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("HeadCollision")));

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

}


