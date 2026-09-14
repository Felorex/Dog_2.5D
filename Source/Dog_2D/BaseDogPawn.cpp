// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDogPawn.h"

// Sets default values
ABaseDogPawn::ABaseDogPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseDogPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseDogPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


