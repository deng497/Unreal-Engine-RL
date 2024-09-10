// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test_TestWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UTest_TestWheelFront::UTest_TestWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}