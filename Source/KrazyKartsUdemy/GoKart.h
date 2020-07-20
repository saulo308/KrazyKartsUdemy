// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

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
	UFUNCTION()
	void SetKartVelocity(float AxisValue);
	UFUNCTION()
	void SetKartSteer(float AxisValue);

	void MoveKart(float DeltaTime);
	void RotateKart(float DeltaTime);
	FVector GetAirResistance();
	FVector GetRollingResistance();

protected:
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

	float Throttle;
	float SteeringThrow;
	FVector KartVelocity;
};
