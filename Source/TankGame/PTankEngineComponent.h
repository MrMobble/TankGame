// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PTankEngineComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TANKGAME_API UPTankEngineComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool Debug;

public:	

	// Sets default values for this component's properties
	UPTankEngineComponent();

	UPROPERTY(EditAnywhere)
	float MaxRPM;

	UPROPERTY(EditAnywhere)
	float IdealRPM;

	//This is a set value instead of a toque curve becuase I want a more arcade style of driving.
	UPROPERTY(EditAnywhere)
	float MaxEngineTorque;

	//This is a set value instead of a toque curve becuase I want a more arcade style of driving.
	UPROPERTY(EditAnywhere)
	float EnginePower;

	UPROPERTY(EditAnywhere)
	float SprocketRadius_CM;

	UPROPERTY(EditAnywhere)
	float SprocketMass_KG;

	UPROPERTY(EditAnywhere)
	float TrackMass_KG;

	UPROPERTY(EditAnywhere)
	float BrakeForce;

	UPROPERTY(EditAnywhere)
	float MuXStatic;

	UPROPERTY(EditAnywhere)
	float MuYStatic;

	UPROPERTY(EditAnywhere)
	float MuXKinetic;

	UPROPERTY(EditAnywhere)
	float MuYKinetic;

	UPROPERTY(EditAnywhere)
	float VehicleMassOverRide;

	// Internal Varibles

public:

	bool ReverseGear;

	float WheelCoefficient_Forward = 0;
	float WheelCoefficient_Left = 0;
	float WheelCoefficient_Right = 0;

	float BreakRatio_Left = 1;
	float BreakRatio_Right = 1;

	float TrackTorqueTransfer_Left;
	float TrackTorqueTransfer_Right;

	//This will always be either 1 or 0;
	float Throttle;

	float DriveTorque_Left;
	float DriveTorque_Right;

	float TrackFrictionTorque_Left;
	float TrackFrictionTorque_Right;

	float TrackRollingFrictionTorque_Left;
	float TrackRollingFrictionTorque_Right;

	float TrackTorque_Left;
	float TrackTorque_Right;

	float MomentInertia;

	float TrackAngularVelocity_Left;
	float TrackAngularVelocity_Right;

	float TrackLinearVelocity_Left;
	float TrackLinearVelocity_Right;

	float AxelAngularVelocity;

	float DriveAxelTorque;

	float EngineTorque;

	FVector DriveForce_Left;
	FVector DriveForce_Right;

	// Calculate Functions

public:

	//Init Functions
	void CalculateMomentOfInertia();

	//Main Functions
	void CalculateThrottle();

	void CalculateWheelVelocity();
		float ApplyBrake(float AngVelIn, float BrakeRatio);

	void CalculateEngineAndDrive();

	FVector2D ApplyDriveForceAndFriction(TArray<class UPWheelComponent*> SusComponents, FVector DriveForce, float TrackLinearVelocity);
		FVector GetVelocity(FVector Location);
		FVector2D CalculateMuFriction(FVector VelocityDirection, FVector ForwardVector, float MuXStatic, float MuYStatic, float MuXKinetic, float MuYKinetic);
		FVector CalculateFullFrictionForce(FVector ForwardVector, FVector RightVector, FVector CollisionNormal, FVector CalculatedForce, float MuX, float MuY);



		
	
};
