// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PWheelComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TANKGAME_API UPWheelComponent : public USceneComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UPWheelComponent();

	UPROPERTY(EditAnywhere)
	class UStaticMesh* Mesh;

	FVector S_RootLocation;
	FRotator S_RootRotation;

	FVector S_WheelLocation;

	FVector S_Force;
	FVector S_CollisionLocation;
	FVector S_CollisionNormal;

	bool S_IsEngaged;

	float S_PreviousLength;

	float S_Length;
	float S_WheelRadius;
	float S_Dampening;
	float S_Stiffness;

};
