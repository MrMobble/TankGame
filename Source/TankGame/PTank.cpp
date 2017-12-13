// Fill out your copyright notice in the Description page of Project Settings.

#include "PTank.h"

//Other Includes
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Math/Quat.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Components/SplineComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "DrawDebugHelpers.h"

//Class Includes
#include "PWheelComponent.h"
#include "PSplineComponent.h"


// Sets default values
APTank::APTank()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	SplineLastIndex = 0.0f;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Base Tank Components
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	TankBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TankBase"));
	TankBase->SetSimulatePhysics(true);
	TankBase->SetMassOverrideInKg(NAME_None, 10400.0f, true);

	RootComponent = TankBase;

	//Create A Camera Boom (Pulls In Towards The Player If There Is A Collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 600.0f;
	CameraBoom->SetRelativeLocation(FVector(0, 0, 80));
	CameraBoom->bUsePawnControlRotation = true;

	//Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Left Suspension Components
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	LS_One = CreateDefaultSubobject<class UPWheelComponent>(TEXT("LS_One"));
	LS_One->SetRelativeLocation(FVector(66, -75, 45));
	LS_One->SetupAttachment(RootComponent);
	SuspensionComponents_Left.Add(LS_One);

	LS_Two = CreateDefaultSubobject<class UPWheelComponent>(TEXT("LS_Two"));
	LS_Two->SetRelativeLocation(FVector(8, -75, 45));
	LS_Two->SetupAttachment(RootComponent);
	SuspensionComponents_Left.Add(LS_Two);

	LS_Three = CreateDefaultSubobject<class UPWheelComponent>(TEXT("LS_Three"));
	LS_Three->SetRelativeLocation(FVector(-53, -75, 45));
	LS_Three->SetupAttachment(RootComponent);
	SuspensionComponents_Left.Add(LS_Three);

	LS_Four = CreateDefaultSubobject<class UPWheelComponent>(TEXT("LS_Four"));
	LS_Four->SetRelativeLocation(FVector(-114, -75, 45));
	LS_Four->SetupAttachment(RootComponent);
	SuspensionComponents_Left.Add(LS_Four);

	LS_Five = CreateDefaultSubobject<class UPWheelComponent>(TEXT("LS_Five"));
	LS_Five->SetRelativeLocation(FVector(-175, -75, 45));
	LS_Five->SetupAttachment(RootComponent);
	SuspensionComponents_Left.Add(LS_Five);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Right Suspension Components
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	RS_One = CreateDefaultSubobject<class UPWheelComponent>(TEXT("RS_One"));
	RS_One->SetRelativeLocation(FVector(66, 75, 45));
	RS_One->SetupAttachment(RootComponent);
	SuspensionComponents_Right.Add(RS_One);

	RS_Two = CreateDefaultSubobject<class UPWheelComponent>(TEXT("RS_Two"));
	RS_Two->SetRelativeLocation(FVector(8, 75, 45));
	RS_Two->SetupAttachment(RootComponent);
	SuspensionComponents_Right.Add(RS_Two);

	RS_Three = CreateDefaultSubobject<class UPWheelComponent>(TEXT("RS_Three"));
	RS_Three->SetRelativeLocation(FVector(-53, 75, 45));
	RS_Three->SetupAttachment(RootComponent);
	SuspensionComponents_Right.Add(RS_Three);

	RS_Four = CreateDefaultSubobject<class UPWheelComponent>(TEXT("RS_Four"));
	RS_Four->SetRelativeLocation(FVector(-114, 75, 45));
	RS_Four->SetupAttachment(RootComponent);
	SuspensionComponents_Right.Add(RS_Four);

	RS_Five = CreateDefaultSubobject<class UPWheelComponent>(TEXT("RS_Five"));
	RS_Five->SetRelativeLocation(FVector(-175, 75, 45));
	RS_Five->SetupAttachment(RootComponent);
	SuspensionComponents_Right.Add(RS_Five);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// TrackMesh
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	LeftTrackMesh = CreateDefaultSubobject<class UInstancedStaticMeshComponent>(TEXT("LT_Mesh"));
	LeftTrackMesh->SetupAttachment(RootComponent);

	RightTrackMesh = CreateDefaultSubobject<class UInstancedStaticMeshComponent>(TEXT("RT_Mesh"));
	RightTrackMesh->SetupAttachment(RootComponent);
}

void APTank::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CalculateMomentOfInertia();

	//InitWheels();
	InitTrackSpline();
	//CreateTrackSpline();
}

//void APTank::BeginPlay()
//{
//	InitTrackSpline();
//	CreateTrackSpline();
//}

void APTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CalculateThrottle();
	CalculateWheelVelocity();
	CalculateEngineAndDrive();

	//Left - Right Suspension Function Call
	for (int32 b = 0; b < SuspensionComponents_Left.Num(); b++) { CalculateSuspension(SuspensionComponents_Left[b]); }
	for (int32 b = 0; b < SuspensionComponents_Right.Num(); b++) { CalculateSuspension(SuspensionComponents_Right[b]); }

	//Left - Right Wheel Function Call
	//for (int32 b = 0; b < WheelComponents_Left.Num(); b++) { SetWheelPosittion(b, WheelComponents_Left[b], true); }
	//for (int32 b = 0; b < WheelComponents_Right.Num(); b++) { SetWheelPosittion(b, WheelComponents_Right[b], false); }

	FVector2D Left = ApplyDriveForceAndFriction(SuspensionComponents_Left, DriveForce_Left, TrackLinearVelocity_Left);
	TrackFrictionTorque_Left = Left.X;
	TrackRollingFrictionTorque_Left = Left.Y;

	FVector2D Right = ApplyDriveForceAndFriction(SuspensionComponents_Right, DriveForce_Right, TrackLinearVelocity_Right);
	TrackFrictionTorque_Right = Right.X;
	TrackRollingFrictionTorque_Right = Right.Y;
}

void APTank::CalculateSuspension(class UPWheelComponent* Suspension)
{
	//Function Variables
	FVector Sus_WorldUpVector;
	FVector Sus_WorldLocation;
	FVector Sus_Force;
	FVector Sus_CollisionLocation;
	FVector Sus_CollisionNormal;
	float Sus_NewLength = 0.0f;

	//Set RelativeTransform To Help Calculate UpVector And WorldLocation
	FTransform SuspensionTransform = Suspension->GetRelativeTransform();
	Sus_WorldUpVector = UKismetMathLibrary::TransformDirection(GetActorTransform(), UKismetMathLibrary::GetUpVector(SuspensionTransform.GetRotation().Rotator()));
	Sus_WorldLocation = UKismetMathLibrary::TransformLocation(GetActorTransform(), SuspensionTransform.GetLocation());

	//Calc Trace End
	FVector TraceEnd = Sus_WorldLocation + ((Sus_WorldUpVector * -1) * Suspension->S_Length);

	//Do Suspension Trace
	FHitResult HitResult = SuspensionTrace(Sus_WorldLocation, TraceEnd, Suspension->S_WheelRadius);

	if (HitResult.bBlockingHit)
	{
		//Set More Variables
		Sus_NewLength = (Sus_WorldLocation - HitResult.Location).Size();
		Sus_CollisionLocation = HitResult.ImpactPoint;
		Sus_CollisionNormal = HitResult.ImpactNormal;

		//Calculate Compression Of Suspension
		float Compression = FMath::Clamp(SpringMacro(Suspension->S_Length, Sus_NewLength), 0.0f, 1.0f);
		float FinalCompression = Compression * Suspension->S_Stiffness;

		//Calculate The Force To Apply To The Tank Suspension
		float ForceC = (Sus_NewLength - Suspension->S_PreviousLength) / GetWorld()->DeltaTimeSeconds;
		float Force = Suspension->S_Dampening * (0 - ForceC);

		//Set The FinalForce Value
		Sus_Force = Sus_WorldUpVector * (FinalCompression + Force);

		//Apply Force To The TankBody
		TankBase->AddForceAtLocation(Sus_Force, Sus_WorldLocation, NAME_None);
	}
	else { Sus_NewLength = Suspension->S_Length; }

	//Set The Suspensions S_NewLength
	Suspension->S_PreviousLength = Sus_NewLength;

	//Set The Current Suspension Force
	Suspension->S_Force = Sus_Force;

	//Set The Suspension S_WheelLocation
	Suspension->S_WheelLocation = HitResult.Location;
	Suspension->S_CollisionLocation = HitResult.ImpactPoint;
	Suspension->S_CollisionNormal = HitResult.ImpactNormal;
	Suspension->S_IsEngaged = HitResult.bBlockingHit;

	if (Debug)
	{
		//DrawDebug Sphere
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), HitResult.Location, Suspension->S_WheelRadius, 12, FColor(255, 255, 0), 0, 1);

		//Debug Force Line
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), Sus_WorldLocation, Sus_WorldLocation + (Sus_Force * 0.00008), FColor(255, 0, 0), 0, 4);
	}
}

FHitResult APTank::SuspensionTrace(FVector Start, FVector End, float Radius)
{
	FCollisionQueryParams TraceParams(TEXT("SusTrace"), true, this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat(), ECC_WorldStatic, FCollisionShape::MakeSphere(Radius), TraceParams);

	return Hit;
}

void APTank::InitWheels()
{
	for (int32 b = 0; b < SuspensionComponents_Left.Num(); b++)
	{
		class UStaticMeshComponent* WheelComponent = NewObject<class UStaticMeshComponent>(this);
		WheelComponent->RegisterComponent();
		WheelComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
		WheelComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WheelComponent->SetRelativeTransform(SuspensionComponents_Left[b]->GetRelativeTransform());
		if (SuspensionComponents_Left[b]->Mesh) { WheelComponent->SetStaticMesh(SuspensionComponents_Left[b]->Mesh); }

		WheelComponents_Left.Add(WheelComponent);
	}

	for (int32 b = 0; b < SuspensionComponents_Right.Num(); b++)
	{
		class UStaticMeshComponent* WheelComponent = NewObject<class UStaticMeshComponent>(this);
		WheelComponent->RegisterComponent();
		WheelComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
		WheelComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WheelComponent->SetRelativeTransform(SuspensionComponents_Right[b]->GetRelativeTransform());
		if (SuspensionComponents_Right[b]->Mesh) { WheelComponent->SetStaticMesh(SuspensionComponents_Right[b]->Mesh); }

		WheelComponents_Right.Add(WheelComponent);
	}
}

void APTank::SetWheelPosittion(int32 Index, class UStaticMeshComponent* WheelComp, bool IsLeft)
{
	//This Function Is Fucked But Works ATM Will Prob Have To Remake And Rethink.

	if (IsLeft)
	{
		FVector WheelTransform = UKismetMathLibrary::TransformLocation(SuspensionComponents_Left[Index]->GetRelativeTransform(), FVector(0, 0, SuspensionComponents_Left[Index]->S_PreviousLength * -1));
		FVector WheelLocation = UKismetMathLibrary::TransformLocation(GetActorTransform(), WheelTransform);
		WheelComp->SetWorldLocationAndRotation(WheelLocation, GetActorRotation());
	}
	
	if (!IsLeft)
	{
		FVector WheelTransform = UKismetMathLibrary::TransformLocation(SuspensionComponents_Right[Index]->GetRelativeTransform(), FVector(0, 0, SuspensionComponents_Right[Index]->S_PreviousLength * -1));
		FVector WheelLocation = UKismetMathLibrary::TransformLocation(GetActorTransform(), WheelTransform);
		WheelComp->SetWorldLocationAndRotation(WheelLocation, GetActorRotation());
	}
}

void APTank::CreateTrackSpline()
{
	InitTrackSpline();

	//Left Spline
	for (int32 b = 0; b < SplineLastIndex; b++)
	{
		if (LeftTrackSpline)
		{
			float DistanceAlongSpline = (LeftTrackSpline->GetSplineLength() / (NumberOfTreads - 1)) * b;

			FVector SplineLocation = LeftTrackSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::Local);

			FRotator SplineRotation = LeftTrackSpline->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::Local);

			FVector SplineRightVector = LeftTrackSpline->GetRightVectorAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::Local);

			FRotator FinalRotation;
			if (SplineRightVector.Y < 0) { FinalRotation = UKismetMathLibrary::MakeRotator(180, SplineRotation.Pitch, SplineRotation.Yaw); }
			else { FinalRotation = UKismetMathLibrary::MakeRotator(SplineRotation.Roll, SplineRotation.Pitch, SplineRotation.Yaw); }

			FTransform FinalTransform = UKismetMathLibrary::MakeTransform(SplineLocation, FinalRotation, FVector(1));
			LeftTrackMesh->AddInstance(FinalTransform);
		}
	}

	//Right Spline
	for (int32 b = 0; b < SplineLastIndex; b++)
	{
		if (RightTrackSpline)
		{
			float DistanceAlongSpline = (RightTrackSpline->GetSplineLength() / (NumberOfTreads - 1)) * b;

			FVector Location = RightTrackSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::Local);
			FRotator Rotation = RightTrackSpline->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::Local);
			FVector RightVector = RightTrackSpline->GetRightVectorAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::Local);

			FRotator FinalRotation;
			if (RightVector.Y <= 0) { FinalRotation = UKismetMathLibrary::MakeRotator(180, Rotation.Pitch, Rotation.Yaw); }
			else { FinalRotation = UKismetMathLibrary::MakeRotator(Rotation.Roll, Rotation.Pitch, Rotation.Yaw); }

			FTransform FinalTransform = UKismetMathLibrary::MakeTransform(Location, FinalRotation, FVector(1));
			RightTrackMesh->AddInstance(FinalTransform);
		}
	}
}

void APTank::InitTrackSpline()
{
	//Set Up SplineLastIndex - 1
	SplineLastIndex = NumberOfTreads - 1;

	//Get All The Custom SplineComponents
	TArray<class UPSplineComponent*> SplineComponents;
	GetComponents<class UPSplineComponent>(SplineComponents);

	//Decide Which Component Is The Left Or Right Spline
	for (int32 b = 0; b < SplineComponents.Num(); b++)
	{
		if (SplineComponents[b]->IsLeft) { LeftTrackSpline = SplineComponents[b]; }
		else if (!SplineComponents[b]->IsLeft) { RightTrackSpline = SplineComponents[b]; }
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Other Random Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Called to bind functionality to input
void APTank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Camera Input
	PlayerInputComponent->BindAxis("Camera-Yaw", this, &APTank::TurnAtRate);
	PlayerInputComponent->BindAxis("Camera-Pitch", this, &APTank::LookUpAtRate);

	//Hook up events for "ZoomIn"
	PlayerInputComponent->BindAxis("Forward/Back", this, &APTank::MoveForward);
	PlayerInputComponent->BindAxis("Left/Right", this, &APTank::MoveRight);
}

//Camera Left/Right && Up/Down
void APTank::TurnAtRate(float Rate) { AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds()); }
void APTank::LookUpAtRate(float Rate) { AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds()); }

void APTank::MoveForward(float AxisValue)
{
	WheelCoefficient_Forward = AxisValue;

	if (AxisValue < 0)
	{
		BreakRatio_Left = -1 * AxisValue;
		BreakRatio_Right = -1 * AxisValue;
	}
	else
	{
		BreakRatio_Left = 0 * AxisValue;
		BreakRatio_Right = 0 * AxisValue;
	}

}

void APTank::MoveRight(float AxisValue)
{
	if (abs(AxisValue * 1) < 0)
	{
		WheelCoefficient_Left = -1 * AxisValue;
		WheelCoefficient_Right = 1 * AxisValue;
	}
	else
	{
		WheelCoefficient_Left = 1 * AxisValue;
		WheelCoefficient_Right = -1 * AxisValue;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ENGINE Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//This Should Be Called From The Tank Script To Init It
void APTank::CalculateMomentOfInertia()
{
	//Sproket Moment Of Inertia * Track Moment Of Inertia
	MomentInertia = ((SprocketMass_KG * 0.5f) * (SprocketRadius_CM * SprocketRadius_CM)) + ((SprocketRadius_CM * SprocketRadius_CM) * TrackMass_KG);
}

//Main Function
void APTank::CalculateThrottle()
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
void APTank::CalculateWheelVelocity()
{
	TrackTorque_Left = (DriveTorque_Left + TrackFrictionTorque_Left) + TrackFrictionTorque_Left;
	TrackTorque_Right = (DriveTorque_Right + TrackFrictionTorque_Right) + TrackFrictionTorque_Right;

	//TrackAngularVelocity_Left = ApplyBrake(((TrackTorque_Left / MomentInertia) * GetWorld()->DeltaTimeSeconds) + TrackAngularVelocity_Left, BreakRatio_Left);
	//TrackAngularVelocity_Right = ApplyBrake(((TrackTorque_Right / MomentInertia) * GetWorld()->DeltaTimeSeconds) + TrackAngularVelocity_Right, BreakRatio_Right);

	float LeftInVelocity = ((TrackTorque_Left / MomentInertia) * GetWorld()->DeltaTimeSeconds) + TrackAngularVelocity_Left;
	float RightInVelocity = ((TrackTorque_Right / MomentInertia) * GetWorld()->DeltaTimeSeconds) + TrackAngularVelocity_Right;
	
	ApplyBrake(LeftInVelocity, BreakRatio_Left, TrackAngularVelocity_Left);
	ApplyBrake(RightInVelocity, BreakRatio_Right, TrackAngularVelocity_Right);

	TrackLinearVelocity_Left = TrackAngularVelocity_Left * SprocketRadius_CM;
	TrackLinearVelocity_Right = TrackAngularVelocity_Right * SprocketRadius_CM;

	UE_LOG(LogTemp, Warning, TEXT("Left: %f Right: %f"), TrackLinearVelocity_Left, TrackLinearVelocity_Right);
}

	float APTank::ApplyBrake(float AngVelIn, float BrakeRatio)
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

void APTank::CalculateEngineAndDrive()
{
	//We * 100 To Turn Meters Into CM
	EngineTorque = GetEngimeTorque(EngineRPM) * Throttle;

	// EnetineTorque * BonusEnginePower
	DriveAxelTorque = EngineTorque * EnginePowerRatio;

	DriveTorque_Left = TrackTorqueTransfer_Left * DriveAxelTorque;
	DriveTorque_Right = TrackTorqueTransfer_Right * DriveAxelTorque;

	DriveForce_Left = ForwardVector(GetActorRotation()) * (DriveTorque_Left / SprocketRadius_CM);
	DriveForce_Right = ForwardVector(GetActorRotation()) * (DriveTorque_Right / SprocketRadius_CM);
}

float APTank::GetEngimeTorque(float RPM)
{
	return RPM * 100;
}

FVector2D APTank::ApplyDriveForceAndFriction(TArray<class UPWheelComponent*> SusComponent, FVector DriveForce, float TrackLinearVelocity)
{
	float TotalTrackFrictionTorque = 0;
	float TotalTrackRollingFrictionTorque = 0;

	FVector FullFrictionNormal;
	FVector FullForce;

	for (int32 b = 0; b < SusComponent.Num(); b++)
	{

		if (SusComponent[b]->S_IsEngaged)
		{

			//Calculate The Current Load On Each Of The Wheels
			float WheelLoad_N = VectorToVectorProject(SusComponent[b]->S_Force, SusComponent[b]->S_CollisionNormal).Size();

			//Calculate The Track Velocity Relative To The Tank
			FVector CalcultedCrap;
			CalcVelo(SusComponent[b]->S_CollisionLocation, CalcultedCrap);

			FVector Velocity = (CalcultedCrap - ForwardVector(GetActorRotation()) * TrackLinearVelocity);
			FVector RelativeTrackVelocity = VectorToPlaneProject(Velocity, SusComponent[b]->S_CollisionNormal);

			//UE_LOG(LogTemp, Warning, TEXT("FullStaticFrictionForce: %s"), *RelativeTrackVelocity.ToString());

			//Calcuate The MuStatic And MuKinetic Store Into Variable
			FVector2D StaticValues = CalculateMuFriction(Normal(RelativeTrackVelocity), ForwardVector(GetActorRotation()), MuXStatic, MuYStatic, MuXKinetic, MuYKinetic);

			//Set The Mu Values
			float MuStatic = StaticValues.X;
			float MuKinetic = StaticValues.Y;

			//Static
			FVector FullStaticFrictionForce =CalculateFullFrictionForce(SusComponent[b]->S_CollisionNormal, RelativeTrackVelocity, MuXStatic, MuYStatic);

			//Kinetic
			FVector FullKineticFrictionForce = CalculateFullFrictionForce(SusComponent[b]->S_CollisionNormal, RelativeTrackVelocity, MuXKinetic, MuYKinetic);

			//UE_LOG(LogTemp, Warning, TEXT("FullStaticFrictionForce: %s"), *Forces[0].ToString());
			//UE_LOG(LogTemp, Warning, TEXT("FullKineticFrictionForce: %s"), *Forces[1].ToString());

			FVector FullStaticDriveForce = UKismetMathLibrary::ProjectVectorOnToPlane(DriveForce, SusComponent[b]->S_CollisionNormal) * MuXStatic;
			FVector FullKineticDriveForce = UKismetMathLibrary::ProjectVectorOnToPlane(DriveForce, SusComponent[b]->S_CollisionNormal) * MuXKinetic;

			//UE_LOG(LogTemp, Warning, TEXT("FullStaticFrictionForce: %s"), *(FullStaticDriveForce + FullKineticDriveForce).ToString());

			if ((FullStaticFrictionForce + FullKineticDriveForce).Size() >= WheelLoad_N * MuStatic)
			{
				FullFrictionNormal = Normal(FullKineticFrictionForce);
				FullForce = UKismetMathLibrary::ClampVectorSize(FullKineticFrictionForce + FullKineticDriveForce, 0.0f, WheelLoad_N * MuKinetic);
				//UE_LOG(LogTemp, Warning, TEXT("Kinetic"));
			}
			if ((FullStaticFrictionForce + FullKineticDriveForce).Size() < WheelLoad_N * MuStatic)
			{
				FullFrictionNormal = Normal(FullStaticFrictionForce);
				FullForce = UKismetMathLibrary::ClampVectorSize(FullStaticFrictionForce + FullStaticDriveForce, 0.0f, WheelLoad_N * MuStatic) / 1.5;
				//UE_LOG(LogTemp, Warning, TEXT("Static"));
			}

			TankBase->AddForceAtLocation(FullForce, SusComponent[b]->S_CollisionLocation, NAME_None);

			FVector TFTValueA = UKismetMathLibrary::ProjectVectorOnToVector(FullForce, FullFrictionNormal) / VehicleMassOverRide;
			FVector TFTValueB = UKismetMathLibrary::InverseTransformDirection(GetTransform(), (TFTValueA * (TrackMass_KG + SprocketMass_KG)) * -1.0f);
			float TFTValueC = UKismetMathLibrary::ProjectVectorOnToVector(TFTValueB, FVector(1.0f, 0.0f, 0.0f)).X;

			float TrackFrictionTorque = TFTValueC * SprocketRadius_CM;

			float TrackRollingFrictionTorque = (WheelLoad_N * 0.02 * (UKismetMathLibrary::SignOfFloat(TrackLinearVelocity) * -1)) + ((UKismetMathLibrary::SignOfFloat(TrackLinearVelocity) * -1) * (TrackLinearVelocity * 0.000015) * WheelLoad_N);

			TotalTrackFrictionTorque = TotalTrackFrictionTorque + TrackFrictionTorque;
			TotalTrackRollingFrictionTorque = TotalTrackRollingFrictionTorque + TrackRollingFrictionTorque;

			//Debug Line Crap
			//DrawDebugLine(GetWorld(), SusComponent[b]->S_CollisionLocation, SusComponent[b]->S_CollisionLocation + (FullForce), FColor(255, 0, 0), false, -1, 0, 15);

			//UE_LOG(LogTemp, Warning, TEXT("FullForce: %s"), *FullForce.ToString());
		}
	}

	return FVector2D(TotalTrackFrictionTorque, TotalTrackRollingFrictionTorque);

}

	//BROKEN FUNCTION
	FVector APTank::GetWorldVelocity(FVector Location)
	{
		FVector vLinearVelocity = TankBase->GetPhysicsLinearVelocity();
		FVector vAngularVelocity = TankBase->GetPhysicsAngularVelocity();
		FVector vCenterOfMass = TankBase->GetCenterOfMass();
		FTransform tActorTransform = GetTransform();
	
		//Inverse The Linear Velocity Using Actors Transform
		FVector vFinalLinearVelocity = InverseTransformVector(tActorTransform, vLinearVelocity);

			FVector vInverseAngular = InverseTransformVector(tActorTransform, vAngularVelocity);
			FVector vInterseAngularToRad = vInverseAngular * 0.0174532925;
			FVector vInverseLocation = InverseTransformLocation(tActorTransform, Location);
			FVector vInverseCOM = InverseTransformLocation(tActorTransform, vCenterOfMass);

		FVector vFinalAngularVelocity = vInterseAngularToRad * (vInverseLocation - vInverseCOM);
		
		return TransformDirection(tActorTransform, (vFinalLinearVelocity + vFinalAngularVelocity));
	}

	//Calculates MuFriction
	FVector2D APTank::CalculateMuFriction(FVector VelocityDirection, FVector ForwardVector, float MuXStatic, float MuYStatic, float MuXKinetic, float MuYKinetic)
	{
		float MuStatic;
		float MuKinetic;

		float DotProduct = UKismetMathLibrary::Dot_VectorVector(VelocityDirection, ForwardVector);

		MuStatic = UKismetMathLibrary::MakeVector2D(MuXStatic * DotProduct, MuYStatic * UKismetMathLibrary::Square((1) - (DotProduct * DotProduct))).Size();
		MuKinetic = UKismetMathLibrary::MakeVector2D(MuXKinetic * DotProduct, MuYKinetic * UKismetMathLibrary::Square((1) - (DotProduct * DotProduct))).Size();

		return FVector2D(MuStatic, MuKinetic);
	}

	//Calculates Full Friction Force
	FVector APTank::CalculateFullFrictionForce(FVector CollisionNormal, FVector RelativeVelocity, float MuX, float MuY)
	{
		FVector vTrackVelocity = (((RelativeVelocity * -1) * VehicleMassOverRide) / GetWorld()->DeltaTimeSeconds) / 10;

		//UE_LOG(LogTemp, Warning, TEXT("vTrackVelocity: %s"), *vTrackVelocity.ToString());

		FVector vVectorATarget = Normal(VectorToPlaneProject(ForwardVector(GetActorRotation()), CollisionNormal));
		FVector vVectorA = VectorToVectorProject(vTrackVelocity, vVectorATarget);

		FVector vVectorBTarget = Normal(VectorToPlaneProject(RightVector(GetActorRotation()), CollisionNormal));
		FVector vVectorB = VectorToVectorProject(vTrackVelocity, vVectorBTarget);

		return (vVectorA * MuX) + (vVectorB * MuY);
	}





















//Math Functions

FVector APTank::VectorToPlaneProject(FVector Vector, FVector PlaneNormal)
{
	return FVector::VectorPlaneProject(Vector, PlaneNormal);
}

FVector APTank::VectorToVectorProject(FVector Vector, FVector Target)
{
	if (Target.SizeSquared() > SMALL_NUMBER)
	{
		return Vector.ProjectOnTo(Target);
	}
	else
	{
		return FVector::ZeroVector;
	}
}

FVector APTank::InverseTransformVector(const FTransform & Transform, FVector Direction)
{
	return Transform.InverseTransformVectorNoScale(Direction);
}

FVector APTank::InverseTransformLocation(const FTransform & Transform, FVector Location)
{
	return Transform.InverseTransformPosition(Location);
}

FVector APTank::TransformDirection(const FTransform & Transform, FVector Direction)
{
	return Transform.TransformVectorNoScale(Direction);
}

FVector APTank::Normal(FVector A)
{
	return A.GetSafeNormal();
}

FVector APTank::ForwardVector(FRotator InRot)
{
	return InRot.Vector();
}

FVector APTank::RightVector(FRotator InRot)
{
	return FRotationMatrix(InRot).GetScaledAxis(EAxis::Y);
}

