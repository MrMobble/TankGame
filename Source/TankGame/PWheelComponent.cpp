// Oliver James Mobbs

#include "PWheelComponent.h"

UPWheelComponent::UPWheelComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	S_RootLocation = GetRelativeTransform().GetLocation();
	S_RootRotation = GetRelativeTransform().GetRotation().Rotator();

	S_WheelLocation = FVector(0, 0, 0);

	S_Force = FVector::ZeroVector;
	S_CollisionLocation = FVector::ZeroVector;
	S_CollisionNormal = FVector::ZeroVector;

	S_PreviousLength = 30.0f;

	S_Length = 30.0f;
	S_WheelRadius = 22.0f;
	S_Dampening = 12000.0f;
	S_Stiffness = 4000000.0f;

	S_IsEngaged = false;
}


