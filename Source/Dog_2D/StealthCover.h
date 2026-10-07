// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "StealthCover.generated.h"


UENUM(BlueprintType)
enum class ECoverType :uint8
{
	Inside UMETA(DisplayName = "Inside"),
	Outside UMETA(DisplayName = "Outside")
};

UINTERFACE(MinimalAPI, Blueprintable)
class UStealthCoverInterface : public UInterface
{
	GENERATED_BODY()
};

class DOG_2D_API IStealthCoverInterface
{
	GENERATED_BODY()

public:

	virtual void SetCurrentCover(class AStealthCover* NewCover) = 0;
};

UCLASS()
class DOG_2D_API AStealthCover : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStealthCover();

	float GetCoverZoneY() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Type")
	ECoverType CoverType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CoverMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* RootCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CoverCollision;

};
