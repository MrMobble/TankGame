// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "PSplineMeshComponent.generated.h"

UCLASS( ClassGroup = (Custom), meta = (BlueprintSpawnableComponent) )
class TANKGAME_API UPSplineMeshComponent : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()

public:

	UPSplineMeshComponent();

	UPROPERTY(EditAnywhere, Category = "SplineSettings")
	bool IsLeft;
	
	
	
	
};
