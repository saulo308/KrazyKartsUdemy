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
	void MoveKart(float DeltaTime);

protected:
	//Mass in kg
	UPROPERTY(EditAnywhere)
	float KartMass = 1000.f;

	//Max driving force applied to car (N)
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000.f;

	float Throttle;
	FVector KartVelocity;
};
