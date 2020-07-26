// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
/* #include "Engine/World.h"
#include "GameFramework/GameStateBase.h" */

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	//MovementComponent
	MovementComponent = CreateDefaultSubobject<UGoKartMovementComponent>(FName("GoKartMovementComponent"));
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

	if(!MovementComponent) return;

	if(Role == ROLE_AutonomousProxy){
		//Creating movement
		FGoKartMove CurrentMove = MovementComponent->CreateMove(DeltaTime);
		//Adding to list of movement
		UnacknowledgedMoves.Add(CurrentMove);
		//Applying movement on server
		ServerApplyKartMove(CurrentMove);
		//Applying movement locally
		MovementComponent->SimulateMove(CurrentMove);
	}

	if(Role == ROLE_Authority && IsLocallyControlled()){
		//Creating movement
		FGoKartMove CurrentMove = MovementComponent->CreateMove(DeltaTime);
		//Applying movement on server
		ServerApplyKartMove(CurrentMove);
	}

	if(Role == ROLE_SimulatedProxy){
		MovementComponent->SimulateMove(ServerState.LastMove);
	}
}

void AGoKart::ClearAcknowledgedMoves(FGoKartMove LastMove){
	TArray<FGoKartMove> NewMoves;

	for(const FGoKartMove& Move : UnacknowledgedMoves){
		if(Move.Time > LastMove.Time)
			NewMoves.Add(Move);
	}

	UnacknowledgedMoves = NewMoves;
}

bool AGoKart::ServerApplyKartMove_Validate(FGoKartMove InKartMove){
	bool bValidForward = FMath::Abs(InKartMove.Throw) <= 1;
	bool bValidSteer = FMath::Abs(InKartMove.SteeringThrow) <= 1;
	return true;
}

void AGoKart::ServerApplyKartMove_Implementation(FGoKartMove InKartMove){
	if(!MovementComponent) return;

	//Received move done by client, simulate it
	MovementComponent->SimulateMove(InKartMove);

	//Update the server state and send back to client(Due to replication)
	ServerState.Transform = GetActorTransform();
	ServerState.KartVelocity = MovementComponent->GetKartVelocity();
	ServerState.LastMove = InKartMove;
}

void AGoKart::OnRep_OnReplicatedServerState(){
	if(!MovementComponent) return;

	//Reseting to server state(position)
	SetActorTransform(ServerState.Transform);
	MovementComponent->SetKartVelocity(ServerState.KartVelocity);

	//Clearing moves stored that are not valid(expired)
	ClearAcknowledgedMoves(ServerState.LastMove);

	//For moves which client is ahead of server, replay all of them.
	for(const FGoKartMove& Move : UnacknowledgedMoves){
		MovementComponent->SimulateMove(Move);
	}
}

void AGoKart::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AGoKart, ServerState);
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