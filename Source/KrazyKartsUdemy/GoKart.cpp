// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Engine/World.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
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

	if(IsLocallyControlled()){
		FGoKartMove NewMove;
		NewMove.Throw = Throttle;
		NewMove.SteeringThrow = SteeringThrow;
		NewMove.DeltaTime = DeltaTime;
		//KartMove.Time = ?;

		//Applying movement on client(AutonomusProxy)
		ServerApplyKartMove(NewMove);
		SimulateMove(NewMove);
	}
}

void AGoKart::SimulateMove(FGoKartMove Move){
	//Moving Kart
	MoveKart(Move.DeltaTime,Move.Throw);
	//Rotating kart
	RotateKart(Move.DeltaTime,Move.SteeringThrow);
}

bool AGoKart::ServerApplyKartMove_Validate(FGoKartMove InKartMove){
	bool bValidForward = FMath::Abs(InKartMove.Throw) <= 1;
	bool bValidSteer = FMath::Abs(InKartMove.SteeringThrow) <= 1;
	return true;
}

void AGoKart::ServerApplyKartMove_Implementation(FGoKartMove InKartMove){
	//Received move done by client, simulate it
	SimulateMove(InKartMove);

	//Update the server state and send back to client(Due to replication)
	ServerState.Transform = GetActorTransform();
	ServerState.KartVelocity = KartVelocity;
	ServerState.LastMove = InKartMove;
}

void AGoKart::OnRep_OnReplicatedServerState(){
	SetActorTransform(ServerState.Transform);
	KartVelocity = ServerState.KartVelocity;
}

void AGoKart::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AGoKart, ServerState);
}

void AGoKart::MoveForward(float AxisValue) {
	//Keeping a member variable to determine ("how much should we apply to driving force") [-1 <=> 1]
	Throttle = AxisValue;
}

void AGoKart::MoveRight(float AxisValue) {
	//Keeping a member variable to determine ("how much should we apply to driving force") [-1 <=> 1]
	SteeringThrow = AxisValue;
}

void AGoKart::MoveKart(float DeltaTime,float InThrow) {
	//Calculating the force that will drive the kart
	//GetActorForwardVector() determines the direction of movement(Forward in this case)
	//(InThrow * MaxDrivingForce determines) how "strong" our force to apply is (from -1 [full backwards force] to 1 [full forward force])
	FVector ForceToApply = GetActorForwardVector() * (InThrow * MaxDrivingForce);

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
	AddActorWorldOffset(Translation,true, &HitResult);
	if (HitResult.IsValidBlockingHit())
		KartVelocity = FVector::ZeroVector;
}

FVector AGoKart::GetAirResistance() {
	//AirResistance = -Speed^2 * DragCoeficient
	//Minus here is been represented by "-KartVelocity.GetSafeNormal()"==Oposite direction of movement
	return -KartVelocity.GetSafeNormal() * KartVelocity.SizeSquared() * DragCoeficient;
}

FVector AGoKart::GetRollingResistance() {
	float Gravity = -GetWorld()->GetGravityZ() / 100.f;
	//N = m x g
	float NormalForce = KartMass * Gravity;
	//Fres = RollingCoef * N
	return -KartVelocity.GetSafeNormal() * RollingCoeficient * NormalForce;
}

void AGoKart::RotateKart(float DeltaTime,float InSteerThrow) {
	//dx = dv * dt
	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(),KartVelocity) * DeltaTime;
	//dx = d0 * r ==> d0 = dx / r
	float RotationDelta = (DeltaLocation / MinTurningRadius) * InSteerThrow;

	//Creating DeltaRotation quaternion
	FQuat DeltaRotation = FQuat(GetActorUpVector(), RotationDelta);

	//Upadating velocity due to rotation
	KartVelocity = DeltaRotation.RotateVector(KartVelocity);

	//Applying rotation
	AddActorWorldRotation(DeltaRotation);
}

