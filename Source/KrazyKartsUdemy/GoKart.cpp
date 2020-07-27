// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bReplicateMovement = false;

	//Componentes
	MovementComponent = CreateDefaultSubobject<UGoKartMovementComponent>(FName("GoKartMovementComponent"));
	MovementReplicator = CreateDefaultSubobject<UGoKartMovementReplicator>(FName("GoKartMovementReplicator"));
}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward",this,&AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);
}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
		NetUpdateFrequency = 1;
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGoKart::MoveForward(float AxisValue) {
	//"how much should we apply to driving force" [-1 <=> 1]
	if(MovementComponent)
		MovementComponent->SetThrottle(AxisValue);
}

void AGoKart::MoveRight(float AxisValue) {
	//"how much should we apply to driving force" [-1 <=> 1]
	if(MovementComponent)
		MovementComponent->SetSteeringThrow(AxisValue);
}