// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "StealthCover.generated.h"


UENUM(BlueprintType)
enum class ECoverType :uint8
{
	Inside UMETA(DisplayName = "Inside"),
	Outside UMETA(DisplayName = "Outside")
};

UCLASS()
class DOG_2D_API AStealthCover : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStealthCover();

	float GetCoverZoneY() const;

	ECoverType GetCoverType() const { return CoverType; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Settings")
	ECoverType CoverType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cover|Settings")
	float OutsideCoverClearanceOffset = 10.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover|Components")
	TObjectPtr<UStaticMeshComponent> CoverMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover|Components")
	TObjectPtr<UBoxComponent> RootCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover|Components")
	TObjectPtr<UBoxComponent> CoverCollision;
};
