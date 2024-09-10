// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test_TestGameMode.h"
#include "Test_TestPlayerController.h"

ATest_TestGameMode::ATest_TestGameMode()
{
	PlayerControllerClass = ATest_TestPlayerController::StaticClass();
}
