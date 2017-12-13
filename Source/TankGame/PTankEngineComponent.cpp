// Fill out your copyright notice in the Description page of Project Settings.

#include "PTankEngineComponent.h"

//Other Includes
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"

//Class Includes
#include "PWheelComponent.h"

// Sets default values for this component's properties
UPTankEngineComponent::UPTankEngineComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

//This Should Be Called From The Tank Script To Init It
void UPTankEngineComponent::CalculateMomentOfInertia()
{
	//Sproket Moment Of Inertia * Track Moment Of Inertia
	MomentInertia = ((SprocketMass_KG * 0.5f) * (SprocketRadius_CM * SprocketRadius_CM)) + ((SprocketRadius_CM * SprocketRadius_CM) * TrackMass_KG);
}

//Main Function
void UPTankEngineComponent::CalculateThrottle()
{
	TrackTorqueTransfer_Left = FMath::Clamp(WheelCoefficient_Left + WheelCoefficient_Forward, -1.0f, 2.0f);
	TrackTorqueTransfer_Right = FMath::Clamp(WheelCoefficient_Right + WheelCoefficient_Forward, -1.0f, 2.0f);

	if (UKismetMathLibrary::Max(abs(TrackTorqueTransfer_Left), abs(TrackTorqueTransfer_Right)) != 0) 
	{ 
		Throttle = 1; 
	}
	else
	{
		Throttle = -1;
		BreakRatio_Left = 1;
		BreakRatio_Right = 1;
	}
}

//Main Function
void UPTankEngineComponent::CalculateWheelVelocity()
{
	TrackTorque_Left = (DriveTorque_Left + TrackFrictionTorque_Left) + TrackFrictionTorque_Left;
	TrackTorque_Right = (DriveTorque_Right + TrackFrictionTorque_Right) + TrackFrictionTorque_Right;

	TrackAngularVelocity_Left = ApplyBrake(((TrackTorque_Left / MomentInertia) * GetWorld()->DeltaTimeSeconds) + TrackAngularVelocity_Left, BreakRatio_Left);
	TrackAngularVelocity_Right = ApplyBrake(((TrackTorque_Right / MomentInertia) * GetWorld()->DeltaTimeSeconds) + TrackAngularVelocity_Right, BreakRatio_Right);

	TrackLinearVelocity_Left = TrackAngularVelocity_Left * SprocketRadius_CM;
	TrackLinearVelocity_Right = TrackAngularVelocity_Right * SprocketRadius_CM;
}
	//Sub Function
	float UPTankEngineComponent::ApplyBrake(float AngVelIn, float BrakeRatio)
	{
		float NewVelocity = 0.0f;
	
		if (abs(AngVelIn) > abs(BrakeRatio * BrakeForce * GetWorld()->DeltaTimeSeconds))
		{
			NewVelocity = (BrakeRatio * BrakeForce * GetWorld()->DeltaTimeSeconds) * UKismetMathLibrary::SignOfFloat(AngVelIn) - AngVelIn;
		}
		else if (abs(AngVelIn) <= abs(BrakeRatio * BrakeForce * GetWorld()->DeltaTimeSeconds))
		{
			NewVelocity = 0.0f;
		}

		return NewVelocity;
	}

void UPTankEngineComponent::CalculateEngineAndDrive()
{
	UPrimitiveComponent* PrimativeComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());

	//TankBody Forward Vector
	FVector ForwardVector = PrimativeComponent->GetForwardVector();

	//We * 100 To Turn Meters Into CM
	EngineTorque = (MaxEngineTorque * 100) * Throttle;

	// EnetineTorque * BonusEnginePower
	DriveAxelTorque = EngineTorque * EnginePower;

	DriveTorque_Left = TrackTorqueTransfer_Left * DriveAxelTorque;
	DriveTorque_Right = TrackTorqueTransfer_Right * DriveAxelTorque;

	DriveForce_Left = ForwardVector * (DriveTorque_Left / SprocketRadius_CM);
	DriveForce_Right = ForwardVector * (DriveTorque_Right / SprocketRadius_CM);

}

FVector2D UPTankEngineComponent::ApplyDriveForceAndFriction(TArray<class UPWheelComponent*> SusComponents, FVector DriveForce, float TrackLinearVelocity)
{
	//All Internal Values Only Needed By The Function
	UPrimitiveComponent* PrimativeComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());

	FVector ForwardVector = PrimativeComponent->GetForwardVector();
	FVector RightVector = PrimativeComponent->GetRightVector();

	float TotalTrackFrictionTorque = 0;
	float TotalTrackRollingFrictionTorque = 0;

	for (int32 b = 0; b < SusComponents.Num(); b++)
	{
		FVector RelativeTrackVelocity;

		float WheelLoad_N;
		float MuStatic;
		float MuKinetic;

		//Friction Force
		FVector FullStaticFrictionForce;
		FVector FullKineticFrictionForce;

		//Drive Force
		FVector FullStaticDriveForce;
		FVector FullKineticDriveForce;

		FVector FullFrictionNormal;
		FVector FullForce;

		if (SusComponents[b]->S_IsEngaged)
		{

			//Calculate The Current Load On Each Of The Wheels
			WheelLoad_N = UKismetMathLibrary::ProjectVectorOnToVector(SusComponents[b]->S_Force, SusComponents[b]->S_CollisionNormal).Size();

			//Calculate The Track Velocity Relative To The Tank
			RelativeTrackVelocity = UKismetMathLibrary::ProjectVectorOnToPlane(GetVelocity(SusComponents[b]->S_CollisionLocation) - (ForwardVector * TrackLinearVelocity), SusComponents[b]->S_CollisionNormal);


			//Calcuate The MuStatic And MuKinetic Store Into Variable
			FVector2D StaticValues = CalculateMuFriction(RelativeTrackVelocity.GetSafeNormal(), ForwardVector, MuXStatic, MuYStatic, MuXKinetic, MuYKinetic);

			//Set The Mu Values
			MuStatic = StaticValues.X;
			MuKinetic = StaticValues.Y;

			UE_LOG(LogTemp, Warning, TEXT("%f + %f"), MuStatic, MuKinetic);

			//Very Long Calculation
			FVector ForceCalculation = (((RelativeTrackVelocity * -1) * 10400.0f) / GetWorld()->DeltaTimeSeconds) / 10;

			//Sub Function Called
			FullStaticFrictionForce = CalculateFullFrictionForce(ForwardVector, RightVector, SusComponents[b]->S_CollisionNormal, ForceCalculation, MuXStatic, MuYStatic);
			FullKineticFrictionForce = CalculateFullFrictionForce(ForwardVector, RightVector, SusComponents[b]->S_CollisionNormal, ForceCalculation, MuXKinetic, MuYKinetic);

			FullStaticDriveForce = UKismetMathLibrary::ProjectVectorOnToPlane(DriveForce, SusComponents[b]->S_CollisionNormal) * MuXStatic;
			FullKineticDriveForce = UKismetMathLibrary::ProjectVectorOnToPlane(DriveForce, SusComponents[b]->S_CollisionNormal) * MuXKinetic;

			float fClampMaxValueStatic = (WheelLoad_N * MuStatic);
			float fClampMaxValueKinetic = (WheelLoad_N * MuKinetic);

			if ((FullStaticFrictionForce + FullStaticDriveForce).Size() >= (WheelLoad_N * MuStatic))
			{
				FullFrictionNormal = UKismetMathLibrary::Normal(FullKineticFrictionForce);
				FullForce = UKismetMathLibrary::ClampVectorSize((FullKineticFrictionForce), 0.0f, fClampMaxValueKinetic);
			}
			if ((FullStaticFrictionForce + FullStaticDriveForce).Size() < (WheelLoad_N * MuStatic))
			{
				FullFrictionNormal = UKismetMathLibrary::Normal(FullStaticFrictionForce);
				FullForce = UKismetMathLibrary::ClampVectorSize((FullStaticFrictionForce), 0.0f, fClampMaxValueStatic);
			}

			PrimativeComponent->AddForceAtLocation(FullForce, SusComponents[b]->S_CollisionLocation, NAME_None);

			//FVector TFTValueA = UKismetMathLibrary::ProjectVectorOnToVector(FullForce, FullFrictionNormal) / VehicleMassOverRide;
			//FVector TFTValueB = UKismetMathLibrary::InverseTransformDirection(GetOwner()->GetTransform(), (TFTValueA * (TrackMass_KG + SprocketMass_KG)) * -1.0f);
			//float TFTValueC = UKismetMathLibrary::ProjectVectorOnToVector(TFTValueB, FVector(1.0f, 0.0f, 0.0f)).X;

			//float TrackFrictionTorque = TFTValueC * SprocketRadius_CM;

			//float TrackRollingFrictionTorque = (WheelLoad_N * 0.02 * (UKismetMathLibrary::SignOfFloat(TrackLinearVelocity) * -1)) +
			//	((UKismetMathLibrary::SignOfFloat(TrackLinearVelocity) * -1) * (TrackLinearVelocity * 0.000015) * WheelLoad_N);

			//TotalTrackFrictionTorque = TotalTrackFrictionTorque + TrackFrictionTorque;
			//TotalTrackRollingFrictionTorque = TotalTrackRollingFrictionTorque + TrackRollingFrictionTorque;

			DrawDebugLine(
				GetWorld(),
				SusComponents[b]->S_CollisionLocation,
				SusComponents[b]->S_CollisionLocation + (FullForce * 0.0015),
				FColor(255, 0, 0),
				false, -1, 0,
				12
			);
		}

	}

	return FVector2D(TotalTrackFrictionTorque, TotalTrackRollingFrictionTorque);
}

	FVector UPTankEngineComponent::GetVelocity(FVector Location)
	{
		UPrimitiveComponent* PrimativeComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
		FTransform ActorTransform = GetOwner()->GetTransform();

		FVector LinearVelocity = PrimativeComponent->GetPhysicsLinearVelocity();
		FVector AngularVelocity = PrimativeComponent->GetPhysicsAngularVelocity();

		FVector FinalLinearVelocity = UKismetMathLibrary::InverseTransformDirection(ActorTransform, LinearVelocity);
		FVector FinalAngularVelocity = (UKismetMathLibrary::InverseTransformDirection(ActorTransform, AngularVelocity) * 0.0174532925) *
		(UKismetMathLibrary::InverseTransformLocation(ActorTransform, Location) - UKismetMathLibrary::InverseTransformLocation(ActorTransform, PrimativeComponent->GetCenterOfMass()));

		FVector FinalVelocity = UKismetMathLibrary::TransformDirection(ActorTransform, FinalLinearVelocity + FinalAngularVelocity);

		return FVector(FinalVelocity);
	}

	FVector2D UPTankEngineComponent::CalculateMuFriction(FVector VelocityDirection, FVector ForwardVector, float MuXStatic, float MuYStatic, float MuXKinetic, float MuYKinetic)
	{
		float MuStatic;
		float MuKinetic;

		float DotProduct = UKismetMathLibrary::Dot_VectorVector(VelocityDirection, ForwardVector);

		MuStatic = UKismetMathLibrary::MakeVector2D(MuXStatic * DotProduct, MuYStatic * UKismetMathLibrary::Square((DotProduct * DotProduct) - 1)).Size();
		MuKinetic = UKismetMathLibrary::MakeVector2D(MuXKinetic * DotProduct, MuYKinetic * UKismetMathLibrary::Square((DotProduct * DotProduct) - 1)).Size();

		return FVector2D(MuStatic, MuKinetic);
	}

	FVector UPTankEngineComponent::CalculateFullFrictionForce(FVector ForwardVector, FVector RightVector, FVector CollisionNormal, FVector CalculatedForce, float MuX, float MuY)
	{
		return FVector(
			(UKismetMathLibrary::ProjectVectorOnToVector(CalculatedForce, UKismetMathLibrary::ProjectVectorOnToPlane(ForwardVector, CollisionNormal).GetSafeNormal()) * MuX) +
			(UKismetMathLibrary::ProjectVectorOnToVector(CalculatedForce, UKismetMathLibrary::ProjectVectorOnToPlane(RightVector, CollisionNormal).GetSafeNormal()) * MuY)
		);
	}

