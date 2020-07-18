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
	//Rotating kart
	RotateKart(DeltaTime);
}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward",this,&AGoKart::SetKartVelocity);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::SetKartSteer);
}

void AGoKart::SetKartVelocity(float AxisValue) {
	//Keeping a member variable to determine ("how much should we apply to driving force") [-1 <=> 1]
	Throttle = AxisValue;
}

void AGoKart::SetKartSteer(float AxisValue) {
	SteeringThrow = AxisValue;
}

void AGoKart::MoveKart(float DeltaTime) {
	//Calculating the force that will drive the kart
	//GetActorForwardVector() determines the direction of movement(Forward in this case)
	//(Throttle * MaxDrivingForce determines) how "strong" our force to apply is (from -1 [full backwards force] to 1 [full forward force])
	FVector ForceToApply = GetActorForwardVector() * (Throttle * MaxDrivingForce);

	//Calculating kart's acceleration: F = m * a   =>   a = F/m
	auto Acceleration = ForceToApply / KartMass;

	//Calculating kart's velocity: a = dv/dt   =>   dv = a * dt
	//However, we could already have a velocity, so "KartVelocity +="
	KartVelocity += Acceleration * DeltaTime;

	//Calculating translation( * 100 to make it cm/s to m/s)
	FVector Translation = KartVelocity * DeltaTime * 100;

	//Translating
	FHitResult HitResult;
	AddActorWorldOffset(Translation,true, &HitResult);
	if (HitResult.IsValidBlockingHit())
		KartVelocity = FVector::ZeroVector;
}

void AGoKart::RotateKart(float DeltaTime) {
	//SteeringThrow gives how much should we rotate (-1 <=> 1)
	float DegreesToRotate = MaxRotationDegreesPerSecond * SteeringThrow * DeltaTime;

	//Creating DeltaRotation quaternion
	FQuat DeltaRotation = FQuat(GetActorUpVector(), FMath::DegreesToRadians(DegreesToRotate));

	//Upadating velocity due to rotation
	KartVelocity = DeltaRotation.RotateVector(KartVelocity);

	//Applying rotation
	AddActorWorldRotation(DeltaRotation);
}

