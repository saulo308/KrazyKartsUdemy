// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "GoKartMovementReplicator.generated.h"

USTRUCT()
struct FGoKartState{
	GENERATED_BODY()

	UPROPERTY()
	FVector KartVelocity;
	UPROPERTY()
	FTransform Transform;
	UPROPERTY()
	FGoKartMove LastMove;
};

struct FHermiteCubicSpline
{
	FVector StartLocation, StartVelocity, TargetLocation, TargetVelocity;

	FVector InterpolateLocation(float LerpRatio) {
		return FMath::CubicInterp(StartLocation,StartVelocity,TargetLocation,TargetVelocity,LerpRatio);
	}

	FVector InterpolateDerivative(float LerpRatio) {
		return FMath::CubicInterpDerivative(StartLocation,StartVelocity,TargetLocation,TargetVelocity,LerpRatio);
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKARTSUDEMY_API UGoKartMovementReplicator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementReplicator();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Replication
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerApplyKartMove(FGoKartMove InKartMove);
	void ClearAcknowledgedMoves(FGoKartMove LastMove);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void ClientTick(float DeltaTime);

	UFUNCTION()
	void OnRep_OnReplicatedServerState();
	void AutonomusProxy_OnRep_OnReplicatedServerState();
	void SimulatedProxy_OnRep_OnReplicatedServerState();

	void UpdateServerState(const FGoKartMove& InKartMove);

	FHermiteCubicSpline CreateSpline(float DerivativeVelocity);
	void InterpolateLocation(FHermiteCubicSpline Spline, float LerpRatio, float DerivativeVelocity);
	void InterpolateVelocity(FHermiteCubicSpline Spline, float LerpRatio, float DerivativeVelocity);
	void InterpolateRotation(float LerpRatio);

private:
	//ServerState(This will be sent by the server down to the clients)
	UPROPERTY(ReplicatedUsing=OnRep_OnReplicatedServerState)
	FGoKartState ServerState;

	//Replay movement system
	TArray<FGoKartMove> UnacknowledgedMoves;
	
	//Linear interpolation
	float ClientTimeSinceUpdate;
	float ClientTimeBetweenLastUpdate;
	FTransform ClientStartTransform;
	FVector ClientStartVelocity;

	float ClientSimulatedTime;

	UPROPERTY()
	UGoKartMovementComponent* MovementComponent = nullptr;
};
