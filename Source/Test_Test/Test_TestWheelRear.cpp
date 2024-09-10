// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test_TestWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UTest_TestWheelRear::UTest_TestWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}