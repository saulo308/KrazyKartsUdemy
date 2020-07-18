// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Moving Kart
	MoveKart(DeltaTime);
}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward",this,&AGoKart::SetKartVelocity);
}

void AGoKart::SetKartVelocity(float AxisValue) {
	KartVelocity = GetActorForwardVector() * AxisValue * MaxKartVelocity;
}

void AGoKart::MoveKart(float DeltaTime) {
	FVector Translation = KartVelocity * 100 * DeltaTime;
	AddActorWorldOffset(Translation);
}

