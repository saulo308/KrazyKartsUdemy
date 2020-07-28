// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.generated.h"

USTRUCT()
struct FGoKartMove{
	GENERATED_BODY()

	UPROPERTY()
	float Throw;
	UPROPERTY()
	float SteeringThrow;
	UPROPERTY()
	float DeltaTime;
	UPROPERTY()
	float Time;

	bool IsValid() {
		return ((FMath::Abs(Throw) <= 1) && (FMath::Abs(SteeringThrow) <= 1));
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKARTSUDEMY_API UGoKartMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SimulateMove(const FGoKartMove& Move);
	FGoKartMove CreateMove(float DeltaTime);

	FORCEINLINE void SetThrottle(float Value){ Throttle = Value; }
	FORCEINLINE void SetSteeringThrow(float Value){ SteeringThrow = Value; }
	FORCEINLINE void SetKartVelocity(FVector NewVelocity) { KartVelocity = NewVelocity; }
	FORCEINLINE FVector GetKartVelocity() { return KartVelocity; }
	FORCEINLINE FGoKartMove GetLastMove() { return LastMove; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void MoveKart(float DeltaTime, float InThrow);
	void RotateKart(float DeltaTime, float InSteerThrow);
	FVector GetAirResistance();
	FVector GetRollingResistance();

private:
	//Mass in kg
	UPROPERTY(EditAnywhere)
	float KartMass = 1000.f;
	//Drag coefficient
	UPROPERTY(EditAnywhere)
	float DragCoeficient = 16.f;//Drag coefficient
	//Rolling Coefficient
	UPROPERTY(EditAnywhere)
	float RollingCoeficient = 0.015f;

	//Max driving force applied to car (N)
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000.f;

	//MinRadius to steer the kart
	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 10.f;

	//Movement properties
	float Throttle;
	float SteeringThrow;
	UPROPERTY()
	FVector KartVelocity;

	FGoKartMove LastMove;
};
