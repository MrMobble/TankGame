// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "PSplineComponent.generated.h"

UCLASS( ClassGroup = (Custom), meta = (BlueprintSpawnableComponent) )
class TANKGAME_API UPSplineComponent : public USplineComponent
{
	GENERATED_BODY()

public:

	UPSplineComponent();

	UPROPERTY(EditAnywhere, Category = "SplineSettings")
	bool IsLeft;
	
};
