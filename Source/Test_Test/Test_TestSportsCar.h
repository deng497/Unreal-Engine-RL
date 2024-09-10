// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Test_TestPawn.h"
#include "Components/SplineComponent.h"
#include "Test_TestSportsCar.generated.h"

/**
 *  Sports car wheeled vehicle implementation
 */
UCLASS(abstract)
class TEST_TEST_API ATest_TestSportsCar : public ATest_TestPawn
{
	GENERATED_BODY()

public:

	ATest_TestSportsCar();

	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	bool FoundManager = false;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	bool InvalidTransform = true;
	
	void ResetToRandomPointOnSpline(USplineComponent* Spline);
};
