// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PTank.generated.h"

UCLASS()
class TANKGAME_API APTank : public APawn
{
	GENERATED_BODY()

	virtual void PostInitializeComponents();

public:

	// Sets default values for this pawn's properties
	APTank();

	//Camera Speed Variables
	float BaseTurnRate;
	float BaseLookUpRate;

	//Camera Turn Functions
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	//Debug Variable
	UPROPERTY(EditAnywhere, Category = "Settings")
	bool Debug;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Base Tank Components
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	//TankBase
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* TankBase;

	//TankTurret
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* TankTurret;

	//TankTurret
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* TankCannon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* BoxCollider;

	//CameraBoom
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USpringArmComponent* CameraBoom;

	//Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* FollowCamera;



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Left Suspension Components
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LeftSuspension")
	class UPWheelComponent* LS_One;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LeftSuspension")
	class UPWheelComponent* LS_Two;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LeftSuspension")
	class UPWheelComponent* LS_Three;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LeftSuspension")
	class UPWheelComponent* LS_Four;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LeftSuspension")
	class UPWheelComponent* LS_Five;

	//Sproket Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprokets")
	class UStaticMeshComponent* LS_SproketFront;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprokets")
	class UStaticMeshComponent* LS_SproketBack;


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Right Suspension Components
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RightSuspension")
	class UPWheelComponent* RS_One;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RightSuspension")
	class UPWheelComponent* RS_Two;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RightSuspension")
	class UPWheelComponent* RS_Three;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RightSuspension")
	class UPWheelComponent* RS_Four;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RightSuspension")
	class UPWheelComponent* RS_Five;

	//Sproket Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprokets")
	class UStaticMeshComponent* RS_SproketFront;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprokets")
	class UStaticMeshComponent* RS_SproketBack;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Suspension Variables / Functions
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	TArray<class UPWheelComponent*> SuspensionComponents_Left;
	TArray<class UPWheelComponent*> SuspensionComponents_Right;
	
	//Main Function
	void CalculateSuspension(class UPWheelComponent* Suspension);

	//Sphere Trace Function
	FHitResult SuspensionTrace(FVector Start, FVector End, float Radius);

	//Math Functions
	float ForceMacro(float A, float B, float C) { return (A * (B - C)); }
	float SpringMacro(float B, float C) { return ((B - C) / B); }

public:

	UFUNCTION(BlueprintCallable, Category = "InitFunction")
	void InitWheels();

	TArray<class UStaticMeshComponent*> WheelComponents_Left;
	TArray<class UStaticMeshComponent*> WheelComponents_Right;

	void SetWheelPosittion(int32 Index, class UStaticMeshComponent* WheelComp, bool IsLeft);

public:

	UPROPERTY(EditAnywhere, Category = "TRACK SETTINGS")
	int32 NumberOfTreads;

	int32 SplineLastIndex;

	void CreateTrackSpline();
	void InitTrackSpline();

	void SetSplineControlPoints(int32 Index, class UPSplineComponent* Spline, class UStaticMeshComponent* WheelComp);
	void UpdateTrackMesh();

	float TreadMeshOffset_Left;
	float TreadMeshOffset_Right;

	//Internal Variables
	class UPSplineComponent* LeftTrackSpline;
	class UPSplineComponent* RightTrackSpline;

	//Imternal Varibles
	class UPSplineMeshComponent* LeftTrackMesh;
	class UPSplineMeshComponent* RightTrackMesh;

	UPROPERTY(EditAnywhere, Category = "TRACK SETTINGS")
	TArray<int32> SplineIndex;

	UPROPERTY(EditAnywhere, Category = "TRACK SETTINGS")
	float WheelRadius;

	UPROPERTY(EditAnywhere, Category = "TRACK SETTINGS")
	float TrackHalfHeight;

	UPROPERTY(EditAnywhere, Category = "TRACK SETTINGS")
	float SproketSpeed;

private:

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	void SetTurretRotation();

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ENGINE Variables / Functions
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	//This is a set value instead of a toque curve becuase I want a more arcade style of driving.
	UPROPERTY(EditAnywhere, Category = "ENGINE VALUES")
		float EngineRPM;

	//This is a set value instead of a toque curve becuase I want a more arcade style of driving.
	UPROPERTY(EditAnywhere, Category = "ENGINE VALUES")
		float EnginePowerRatio;

	//This is a set value instead of a toque curve becuase I want a more arcade style of driving.
	UPROPERTY(EditAnywhere, Category = "ENGINE VALUES")
		float EngineSpeedLimit;

	UPROPERTY(EditAnywhere, Category = "ENGINE VALUES")
		float SprocketRadius_CM;

	UPROPERTY(EditAnywhere, Category = "ENGINE VALUES")
		float SprocketMass_KG;

	UPROPERTY(EditAnywhere, Category = "ENGINE VALUES")
		float TrackMass_KG;

	UPROPERTY(EditAnywhere, Category = "ENGINE VALUES")
		float BrakeForce;

	UPROPERTY(EditAnywhere, Category = "ENGINE VALUES")
		float MuXStatic;

	UPROPERTY(EditAnywhere, Category = "ENGINE VALUES")
		float MuYStatic;

	UPROPERTY(EditAnywhere, Category = "ENGINE VALUES")
		float MuXKinetic;

	UPROPERTY(EditAnywhere, Category = "ENGINE VALUES")
		float MuYKinetic;

	UPROPERTY(EditAnywhere, Category = "ENGINE VALUES")
		float VehicleMassOverRide;

	UPROPERTY(EditAnywhere, Category = "ENGINE VALUES")
		bool UseThrottle;

	UPROPERTY(EditAnywhere, Category = "ENGINE VALUES")
		float ThrottleIncrement;

	UPROPERTY(EditAnywhere, Category = "ENGINE SOUNDS")
		class USoundCue* EngineStartUp;

	UPROPERTY(EditAnywhere, Category = "ENGINE SOUNDS")
		class USoundCue* EngineIdle;

	UPROPERTY(EditAnywhere, Category = "ENGINE SOUNDS")
		class USoundCue* Engine;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AudioComponent")
		class UAudioComponent* EngineAudioComponent;

	// Internal Varibles

public:

	bool ReverseGear;

	float WheelCoefficient_Forward;
	float WheelCoefficient_Left;
	float WheelCoefficient_Right;

	float BreakRatio_Left;
	float BreakRatio_Right;

	UPROPERTY(BlueprintReadOnly)
	float TrackTorqueTransfer_Left;

	UPROPERTY(BlueprintReadOnly)
	float TrackTorqueTransfer_Right;

	//This will always be either 1 or 0;
	UPROPERTY(BlueprintReadOnly)
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
		float GetEngimeTorque(float RPM);

	FVector2D ApplyDriveForceAndFriction(TArray<class UPWheelComponent*> SusComponent, FVector DriveForce, float TrackLinearVelocity);
		FVector GetWorldVelocity(FVector Location);
		FVector2D CalculateMuFriction(FVector VelocityDirection, FVector ForwardVector, float MuXStatic, float MuYStatic, float MuXKinetic, float MuYKinetic);
		FVector CalculateFullFrictionForce(FVector CollisionNormal, FVector RelativeVelocity, float MuX, float MuY);

		UFUNCTION(BlueprintImplementableEvent, Category = Tracking)
		void CalcVelo(FVector Vector, FVector& Result);

		UFUNCTION(BlueprintImplementableEvent, Category = Tracking)
		void ApplyBrake(float AngVelIn, float BrakeRatio, float& Result);

	//Simple Math Functions

public:

	FVector VectorToPlaneProject(FVector Vector, FVector PlaneNormal);

	FVector VectorToVectorProject(FVector Vector, FVector Target);

	FVector InverseTransformVector(const FTransform& Transform, FVector Direction);

	FVector InverseTransformLocation(const FTransform& Transform, FVector Location);

	FVector TransformDirection(const FTransform& Transform, FVector Direction);

	FVector Normal(FVector A);

	FVector ForwardVector(FRotator InRot);

	FVector RightVector(FRotator InRot);

	
	
};
