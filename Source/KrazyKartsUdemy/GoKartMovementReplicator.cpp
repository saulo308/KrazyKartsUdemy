// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKartMovementReplicator.h"

// Sets default values for this component's properties
UGoKartMovementReplicator::UGoKartMovementReplicator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	// ...
}

// Called when the game starts
void UGoKartMovementReplicator::BeginPlay()
{
	Super::BeginPlay();
	MovementComponent = GetOwner()->FindComponentByClass<UGoKartMovementComponent>();
}

// Called every frame
void UGoKartMovementReplicator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!MovementComponent) return;

	FGoKartMove LastMove = MovementComponent->GetLastMove();

	if(GetOwnerRole() == ROLE_AutonomousProxy){
		//Adding to list of movement
		UnacknowledgedMoves.Add(LastMove);
		//Applying movement on server
		ServerApplyKartMove(LastMove);
	}

	if(GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy){
		UpdateServerState(LastMove);
	}

	if(GetOwnerRole() == ROLE_SimulatedProxy){
		ClientTick(DeltaTime);
	}
}

void UGoKartMovementReplicator::ClientTick(float DeltaTime){
	ClientTimeSinceUpdate += DeltaTime;
	if(ClientTimeBetweenLastUpdate < KINDA_SMALL_NUMBER) return;
	if(!MovementComponent) return;
	/* //Linear Interpolation
	auto TargetLocation = ServerState.Transform.GetLocation();
	auto TargetRotation = ServerState.Transform.GetRotation();
	auto LerpRatio = ClientTimeSinceUpdate / ClientTimeBetweenLastUpdate;
	auto NextLocation = FMath::LerpStable(ClientStartTransform.GetLocation(),TargetLocation,LerpRatio);
	auto NextRotation = FQuat::Slerp(ClientStartTransform.GetRotation(),TargetRotation,LerpRatio);

	GetOwner()->SetActorLocation(NextLocation);
	GetOwner()->SetActorRotation(NextRotation); */

	//Location
	auto LerpRatio = ClientTimeSinceUpdate / ClientTimeBetweenLastUpdate;
	float DerivativeVelocity = ClientTimeBetweenLastUpdate * 100;

	auto StartLocation = ClientStartTransform.GetLocation();
	auto StartVelocity = ClientStartVelocity * DerivativeVelocity;

	auto TargetLocation = ServerState.Transform.GetLocation();
	auto TargetVelocity = ServerState.KartVelocity * DerivativeVelocity;

	auto NewLocation = FMath::CubicInterp(StartLocation,StartVelocity,TargetLocation,TargetVelocity,LerpRatio);
	GetOwner()->SetActorLocation(NewLocation);

	//Velocity
	auto NewDerivative = FMath::CubicInterpDerivative(StartLocation,StartVelocity,TargetLocation,TargetVelocity,LerpRatio);
	auto NewVelocity = NewDerivative / DerivativeVelocity;
	MovementComponent->SetKartVelocity(NewVelocity);

	//Rotation
	auto TargetRotation = ServerState.Transform.GetRotation();
	auto NextRotation = FQuat::Slerp(ClientStartTransform.GetRotation(),TargetRotation,LerpRatio);
	GetOwner()->SetActorRotation(NextRotation);
}

void UGoKartMovementReplicator::ClearAcknowledgedMoves(FGoKartMove LastMove){
	TArray<FGoKartMove> NewMoves;

	for(const FGoKartMove& Move : UnacknowledgedMoves){
		if(Move.Time > LastMove.Time)
			NewMoves.Add(Move);
	}

	UnacknowledgedMoves = NewMoves;
}

bool UGoKartMovementReplicator::ServerApplyKartMove_Validate(FGoKartMove InKartMove){
	bool bValidForward = FMath::Abs(InKartMove.Throw) <= 1;
	bool bValidSteer = FMath::Abs(InKartMove.SteeringThrow) <= 1;
	return true;
}

void UGoKartMovementReplicator::ServerApplyKartMove_Implementation(FGoKartMove InKartMove){
	if(!MovementComponent) return;

	//Received move done by client, simulate it
	MovementComponent->SimulateMove(InKartMove);
	UpdateServerState(InKartMove);
}

void UGoKartMovementReplicator::OnRep_OnReplicatedServerState(){
	switch(GetOwnerRole()){
		case ROLE_AutonomousProxy:
			AutonomusProxy_OnRep_OnReplicatedServerState();
			break;
		case ROLE_SimulatedProxy:
			SimulatedProxy_OnRep_OnReplicatedServerState();
			break;
	}
}

void UGoKartMovementReplicator::AutonomusProxy_OnRep_OnReplicatedServerState(){
	if(!MovementComponent) return;

	//Reseting to server state(position)
	GetOwner()->SetActorTransform(ServerState.Transform);
	MovementComponent->SetKartVelocity(ServerState.KartVelocity);

	//Clearing moves stored that are not valid(expired)
	ClearAcknowledgedMoves(ServerState.LastMove);

	//For moves which client is ahead of server, replay all of them.
	for(const FGoKartMove& Move : UnacknowledgedMoves){
		MovementComponent->SimulateMove(Move);
	}
}

void UGoKartMovementReplicator::SimulatedProxy_OnRep_OnReplicatedServerState(){
	if(!MovementComponent) return;

	ClientTimeBetweenLastUpdate = ClientTimeSinceUpdate; 
	ClientTimeSinceUpdate = 0;

	ClientStartTransform = GetOwner()->GetActorTransform();
	ClientStartVelocity = MovementComponent->GetKartVelocity();
}

void UGoKartMovementReplicator::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UGoKartMovementReplicator, ServerState);
}

void UGoKartMovementReplicator::UpdateServerState(const FGoKartMove& InKartMove){
	//Update the server state and send back to client(Due to replication)
	ServerState.Transform = GetOwner()->GetActorTransform();
	ServerState.KartVelocity = MovementComponent->GetKartVelocity();
	ServerState.LastMove = InKartMove;
}