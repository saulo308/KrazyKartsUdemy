// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "GoKartMovementComponent.h"
#include "GoKart.generated.h"

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

UCLASS()
class KRAZYKARTSUDEMY_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Forward Movement
	void MoveForward(float AxisValue);

	//Right Movement
	void MoveRight(float AxisValue);

private:
	void ClearAcknowledgedMoves(FGoKartMove LastMove);

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerApplyKartMove(FGoKartMove InKartMove);

	//ServerState(This will be sent by the server down to the clients)
	UPROPERTY(ReplicatedUsing=OnRep_OnReplicatedServerState)
	FGoKartState ServerState;
	UFUNCTION()
	void OnRep_OnReplicatedServerState();

	//Replay movement system
	TArray<FGoKartMove> UnacknowledgedMoves;

private:
	UPROPERTY(EditAnywhere)
	UGoKartMovementComponent* MovementComponent = nullptr;
};
