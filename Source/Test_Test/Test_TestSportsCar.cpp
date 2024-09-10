// Copyright Epic Games, Inc. All Rights Reserved.


#include "Test_TestSportsCar.h"
#include "Test_TestSportsWheelFront.h"
#include "Test_TestSportsWheelRear.h"
#include "Kismet/KismetMathLibrary.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "LearningAgentsManager.h"
#include "Kismet/GameplayStatics.h"

ATest_TestSportsCar::ATest_TestSportsCar()
{
	// Note: for faster iteration times, the vehicle setup can be tweaked in the Blueprint instead

	// Set up the chassis
	GetChaosVehicleMovement()->ChassisHeight = 144.0f;
	GetChaosVehicleMovement()->DragCoefficient = 0.31f;

	// Set up the wheels
	GetChaosVehicleMovement()->bLegacyWheelFrictionPosition = true;
	GetChaosVehicleMovement()->WheelSetups.SetNum(4);

	GetChaosVehicleMovement()->WheelSetups[0].WheelClass = UTest_TestSportsWheelFront::StaticClass();
	GetChaosVehicleMovement()->WheelSetups[0].BoneName = FName("Phys_Wheel_FL");
	GetChaosVehicleMovement()->WheelSetups[0].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f);

	GetChaosVehicleMovement()->WheelSetups[1].WheelClass = UTest_TestSportsWheelFront::StaticClass();
	GetChaosVehicleMovement()->WheelSetups[1].BoneName = FName("Phys_Wheel_FR");
	GetChaosVehicleMovement()->WheelSetups[1].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f);

	GetChaosVehicleMovement()->WheelSetups[2].WheelClass = UTest_TestSportsWheelRear::StaticClass();
	GetChaosVehicleMovement()->WheelSetups[2].BoneName = FName("Phys_Wheel_BL");
	GetChaosVehicleMovement()->WheelSetups[2].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f);

	GetChaosVehicleMovement()->WheelSetups[3].WheelClass = UTest_TestSportsWheelRear::StaticClass();
	GetChaosVehicleMovement()->WheelSetups[3].BoneName = FName("Phys_Wheel_BR");
	GetChaosVehicleMovement()->WheelSetups[3].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f);

	// Set up the engine
	// NOTE: Check the Blueprint asset for the Torque Curve
	GetChaosVehicleMovement()->EngineSetup.MaxTorque = 750.0f;
	GetChaosVehicleMovement()->EngineSetup.MaxRPM = 7000.0f;
	GetChaosVehicleMovement()->EngineSetup.EngineIdleRPM = 900.0f;
	GetChaosVehicleMovement()->EngineSetup.EngineBrakeEffect = 0.2f;
	GetChaosVehicleMovement()->EngineSetup.EngineRevUpMOI = 5.0f;
	GetChaosVehicleMovement()->EngineSetup.EngineRevDownRate = 600.0f;

	// Set up the transmission
	GetChaosVehicleMovement()->TransmissionSetup.bUseAutomaticGears = true;
	GetChaosVehicleMovement()->TransmissionSetup.bUseAutoReverse = true;
	GetChaosVehicleMovement()->TransmissionSetup.FinalRatio = 2.81f;
	GetChaosVehicleMovement()->TransmissionSetup.ChangeUpRPM = 6000.0f;
	GetChaosVehicleMovement()->TransmissionSetup.ChangeDownRPM = 2000.0f;
	GetChaosVehicleMovement()->TransmissionSetup.GearChangeTime = 0.2f;
	GetChaosVehicleMovement()->TransmissionSetup.TransmissionEfficiency = 0.9f;

	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios.SetNum(5);
	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[0] = 4.25f;
	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[1] = 2.52f;
	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[2] = 1.66f;
	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[3] = 1.22f;
	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[4] = 1.0f;

	GetChaosVehicleMovement()->TransmissionSetup.ReverseGearRatios.SetNum(1);
	GetChaosVehicleMovement()->TransmissionSetup.ReverseGearRatios[0] = 4.04f;

	// Set up the steering
	// NOTE: Check the Blueprint asset for the Steering Curve
	GetChaosVehicleMovement()->SteeringSetup.SteeringType = ESteeringType::Ackermann;
	GetChaosVehicleMovement()->SteeringSetup.AngleRatio = 0.7f;
}

void ATest_TestSportsCar::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("Start"));

	UWorld* World = GetWorld();
	if (World)
	{
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsWithTag(World, FName(TEXT("LearningAgentsManager")), OutActors);
		// 现在 OutActors 中包含了所有带有“LearningAgentsManager”标签的 Actor

		for(auto &Out_Actors:OutActors)
		{
			if(ULearningAgentsManager* LearningAgentsManager = Cast<ULearningAgentsManager>(Out_Actors->GetComponentByClass(ULearningAgentsManager::StaticClass())))
			{
				LearningAgentsManager->AddAgent(this);
				FoundManager = true;
			}
		}
	}
	
	
	if(!FoundManager)
	{
		UE_LOG(LogTemp, Log, TEXT("None"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Yes"));
	}
	
}

void ATest_TestSportsCar::ResetToRandomPointOnSpline(USplineComponent* Spline)
{
    FTransform CandidateTransform;
    bool bValidTransform = false;

    // 随机生成直到找到有效的候选变换
    while (!bValidTransform)
    {
        // 生成随机的X和Y偏移量
        float RandX = (UKismetMathLibrary::RandomFloat() - 0.5f) * 1200.0f;
        float RandY = (UKismetMathLibrary::RandomFloat() - 0.5f) * 1200.0f;

        FVector RandOffset = FVector(RandX, RandY, 50.0f);

        // 在样条曲线上随机选择一个位置
        float RandSplineDistance = Spline->GetSplineLength() * UKismetMathLibrary::RandomFloat();
        FVector RandLocationOnSpline = Spline->GetLocationAtDistanceAlongSpline(RandSplineDistance, ESplineCoordinateSpace::World);

        FVector FinalLocation = RandLocationOnSpline + RandOffset;

        // 获取样条曲线上的旋转
        FRotator RandRotator = Spline->GetRotationAtDistanceAlongSpline(RandSplineDistance, ESplineCoordinateSpace::World);

        // 随机调整Yaw旋转
        RandRotator.Yaw += (UKismetMathLibrary::RandomFloat() - 0.5f) * 90.0f;
        RandRotator.Pitch = 0.0f;
        RandRotator.Roll = 0.0f;

        // 构建变换矩阵
        CandidateTransform = FTransform(RandRotator, FinalLocation, FVector(1.0f, 1.0f, 1.0f));

        bValidTransform = true;

        // 获取场景中的所有SportsCar对象
        TArray<AActor*> OutActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATest_TestSportsCar::StaticClass(), OutActors);

        // 检查其他车辆的距离是否符合要求
        for (AActor* OtherActor : OutActors)
        {
            FVector OtherLocation = OtherActor->GetActorLocation();
            float Distance = FVector::Dist(CandidateTransform.GetLocation(), OtherLocation);

            // 如果与其他车辆太近，重新生成位置
            if (Distance < 1500.0f)
            {
                bValidTransform = false;
                break;
            }
        }
    }

    // 将车辆传送到随机生成的位置，使用 ETeleportType::TeleportPhysics 标志
    SetActorTransform(CandidateTransform, false, nullptr, ETeleportType::TeleportPhysics);

    // 重置物理属性（角速度和线速度）
    GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
    GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
}

