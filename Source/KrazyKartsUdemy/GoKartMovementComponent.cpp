// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKartMovementComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"

// Sets default values for this component's properties
UGoKartMovementComponent::UGoKartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UGoKartMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
	
}

// Called every frame
void UGoKartMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ..
}

void UGoKartMovementComponent::SimulateMove(const FGoKartMove& Move){
	//Moving Kart
	MoveKart(Move.DeltaTime,Move.Throw);
	//Rotating kart
	RotateKart(Move.DeltaTime,Move.SteeringThrow);
}

FGoKartMove UGoKartMovementComponent::CreateMove(float DeltaTime){
	FGoKartMove NewMove;
	NewMove.Throw = Throttle;
	NewMove.SteeringThrow = SteeringThrow;
	NewMove.DeltaTime = DeltaTime;
	NewMove.Time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

	return NewMove;
}


void UGoKartMovementComponent::MoveKart(float DeltaTime,float InThrow) {
	//Calculating the force that will drive the kart
	//GetActorForwardVector() determines the direction of movement(Forward in this case)
	//(InThrow * MaxDrivingForce determines) how "strong" our force to apply is (from -1 [full backwards force] to 1 [full forward force])
	FVector ForceToApply = GetOwner()->GetActorForwardVector() * (InThrow * MaxDrivingForce);

	//Getting AirResistance
	ForceToApply += GetAirResistance();

	//Rolling resistance
	ForceToApply += GetRollingResistance();

	//Calculating kart's acceleration: F = m * a   =>   a = F/m
	auto Acceleration = ForceToApply / KartMass;

	//Calculating kart's velocity: a = dv/dt   =>   dv = a * dt
	//However, we could already have a velocity, so "KartVelocity +="
	KartVelocity += Acceleration * DeltaTime;

	//Calculating translation( * 100 to make it cm/s to m/s)
	FVector Translation = KartVelocity * DeltaTime * 100;

	//Translating
	FHitResult HitResult;
	GetOwner()->AddActorWorldOffset(Translation,true, &HitResult);
	if (HitResult.IsValidBlockingHit())
		KartVelocity = FVector::ZeroVector;
}

FVector UGoKartMovementComponent::GetAirResistance() {
	//AirResistance = -Speed^2 * DragCoeficient
	//Minus here is been represented by "-KartVelocity.GetSafeNormal()"==Oposite direction of movement
	return -KartVelocity.GetSafeNormal() * KartVelocity.SizeSquared() * DragCoeficient;
}

FVector UGoKartMovementComponent::GetRollingResistance() {
	float Gravity = -GetOwner()->GetWorld()->GetGravityZ() / 100.f;
	//N = m x g
	float NormalForce = KartMass * Gravity;
	//Fres = RollingCoef * N
	return -KartVelocity.GetSafeNormal() * RollingCoeficient * NormalForce;
}

void UGoKartMovementComponent::RotateKart(float DeltaTime,float InSteerThrow) {
	//dx = dv * dt
	float DeltaLocation = FVector::DotProduct(GetOwner()->GetActorForwardVector(),KartVelocity) * DeltaTime;
	//dx = d0 * r ==> d0 = dx / r
	float RotationDelta = (DeltaLocation / MinTurningRadius) * InSteerThrow;

	//Creating DeltaRotation quaternion
	FQuat DeltaRotation = FQuat(GetOwner()->GetActorUpVector(), RotationDelta);

	//Upadating velocity due to rotation
	KartVelocity = DeltaRotation.RotateVector(KartVelocity);

	//Applying rotation
	GetOwner()->AddActorWorldRotation(DeltaRotation);
}
