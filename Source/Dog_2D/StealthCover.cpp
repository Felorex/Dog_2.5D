// Fill out your copyright notice in the Description page of Project Settings.


#include "StealthCover.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AStealthCover::AStealthCover()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RootCollision"));
	RootComponent = RootCollision;

	CoverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	CoverMesh->SetupAttachment(RootComponent);

	CoverCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CoverCollision"));
	CoverCollision->SetupAttachment(RootComponent);

	CoverCollision->SetGenerateOverlapEvents(true);

	CoverType = ECoverType::Outside;
}

void AStealthCover::NotifyActorBeginOverlap(AActor* OtherActor)
{
	AActor::NotifyActorBeginOverlap(OtherActor);

	if (OtherActor)
	{
		IStealthCoverInterface* StealthAgent = Cast<IStealthCoverInterface>(OtherActor);
		if (StealthAgent)
		{
			StealthAgent->SetCurrentCover(this);
		}
	}
}

void AStealthCover::NotifyActorEndOverlap(AActor* OtherActor)
{
	AActor::NotifyActorEndOverlap(OtherActor);

	if (OtherActor)
	{
		IStealthCoverInterface* StealthAgent = Cast<IStealthCoverInterface>(OtherActor);
		if (StealthAgent)
		{
			StealthAgent->SetCurrentCover(nullptr);
		}
	}
}


float AStealthCover::GetCoverZoneY() const
{
	if (RootCollision)
	{
		FVector BoxLocation = RootCollision->GetComponentLocation();
		FVector BoxExtent = RootCollision->Bounds.BoxExtent;
		if (CoverType == ECoverType::Inside)
		{
			return BoxLocation.Y;
		}
		else // ECoverType::Outside
		{
			return BoxLocation.Y + BoxExtent.Y + 10.f;
		}
	}
	return GetActorLocation().Y;
}
// Called when the game starts or when spawned
void AStealthCover::BeginPlay()
{
	Super::BeginPlay();	

}


