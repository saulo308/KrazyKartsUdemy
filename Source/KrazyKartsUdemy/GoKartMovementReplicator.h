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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKARTSUDEMY_API UGoKartMovementReplicator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementReplicator();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_OnReplicatedServerState();

	//Replication
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerApplyKartMove(FGoKartMove InKartMove);
	void ClearAcknowledgedMoves(FGoKartMove LastMove);

private:
	//ServerState(This will be sent by the server down to the clients)
	UPROPERTY(ReplicatedUsing=OnRep_OnReplicatedServerState)
	FGoKartState ServerState;

	//Replay movement system
	TArray<FGoKartMove> UnacknowledgedMoves;

	UPROPERTY()
	UGoKartMovementComponent* MovementComponent = nullptr;
};
