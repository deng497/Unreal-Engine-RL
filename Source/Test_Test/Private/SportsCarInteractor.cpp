// Fill out your copyright notice in the Description page of Project Settings.


#include "SportsCarInteractor.h"

#include "ChaosVehicleMovementComponent.h"
#include "K2Node_MakeMap.h"
#include "Components/SplineComponent.h"
#include "Test_Test/Test_TestSportsCar.h"

USportsCarInteractor::USportsCarInteractor()
{
	TrackSpline = CreateDefaultSubobject<USplineComponent>("TrackSpline");
}

void USportsCarInteractor::SpecifyAgentObservation_Implementation(FLearningAgentsObservationSchemaElement& OutObservationSchemaElement,
                                                   ULearningAgentsObservationSchema* InObservationSchema)
{
    // static FLearningAgentsObservationSchemaElement SpecifyLocationAlongSplineObservation(ULearningAgentsObservationSchema* Schema, const FName Tag = TEXT("LocationAlongSplineObservation"));
    // static FLearningAgentsObservationSchemaElement SpecifyStructObservation(ULearningAgentsObservationSchema* Schema, const TMap<FName, FLearningAgentsObservationSchemaElement>& Elements, const FName Tag = TEXT("StructObservation"));
    // static FLearningAgentsObservationSchemaElement SpecifyVelocityObservation(ULearningAgentsObservationSchema* Schema, const FName Tag = TEXT("VelocityObservation"));

    // 获取基于样条曲线位置的观察元素。
    FLearningAgentsObservationSchemaElement LearningAgentsObservationLocation = ULearningAgentsObservations::SpecifyLocationAlongSplineObservation(InObservationSchema);

    // 获取基于样条曲线方向的观察元素。
    FLearningAgentsObservationSchemaElement LearningAgentsObservationDirection = ULearningAgentsObservations::SpecifyDirectionAlongSplineObservation(InObservationSchema);

    // 获取速度的观察元素。
    FLearningAgentsObservationSchemaElement LearningAgentsObservationVelocity = ULearningAgentsObservations::SpecifyVelocityObservation(InObservationSchema);

    // 创建一个 TMap 用于存储观察元素的键值对。
    TMap<FName,FLearningAgentsObservationSchemaElement> Elements;

    // 向 Map 中添加位置观察元素，键为"Location"。
    Elements.Add(TEXT("Location"),LearningAgentsObservationLocation);

    // 向 Map 中添加方向观察元素，键为"Direction"。
    Elements.Add(TEXT("Direction"),LearningAgentsObservationDirection);

    // 使用指定的观察模式和已有的元素 Map 创建一个结构体观察元素，可能表示轨道相关的观察。
    FLearningAgentsObservationSchemaElement LearningAgentsObservationTrack = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema,Elements);

    // 清空 Map，准备存储新的元素。
    Elements.Empty();

    // 向 Map 中添加轨道观察元素，键为"Track"。
    Elements.Add(TEXT("Track"),LearningAgentsObservationTrack);

    // 向 Map 中添加速度观察元素，键为"Car"。
    Elements.Add(TEXT("Car"),LearningAgentsObservationVelocity);

    // 使用指定的观察模式和新的元素 Map 创建最终的观察元素结构体，并将其赋值给输出参数。
    OutObservationSchemaElement = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema,Elements);
}

void USportsCarInteractor::GatherAgentObservation_Implementation(FLearningAgentsObservationObjectElement& OutObservationObjectElement,
	ULearningAgentsObservationObject* InObservationObject, const int32 AgentId)
{
	//	UFUNCTION(BlueprintPure, Category = "LearningAgents", meta = (AgentId = "-1", DeterminesOutputType = "AgentClass"))
	//UObject* GetAgent(const int32 AgentId, const TSubclassOf<UObject> AgentClass) const;
	
	UObject* agentObject = GetAgent(AgentId,ATest_TestSportsCar::StaticClass());
	check(agentObject);

	AActor* ObsActor = Cast<AActor>(agentObject);

	float DistanceAloneSpline =  TrackSpline->GetDistanceAlongSplineAtLocation(ObsActor->GetActorLocation(),ESplineCoordinateSpace::World);
	FLearningAgentsObservationObjectElement Location = ULearningAgentsObservations::MakeLocationAlongSplineObservation(InObservationObject,TrackSpline,DistanceAloneSpline,ObsActor->GetTransform(),10000.0);
	FLearningAgentsObservationObjectElement Direction = ULearningAgentsObservations::MakeDirectionAlongSplineObservation(InObservationObject,TrackSpline,DistanceAloneSpline,ObsActor->GetTransform());
	
	TMap<FName,FLearningAgentsObservationObjectElement> Elements;
	Elements.Add(TEXT("Location"),Location);
	Elements.Add(TEXT("Direction"),Direction);

	FLearningAgentsObservationObjectElement Track =  ULearningAgentsObservations::MakeStructObservation(InObservationObject,Elements);
	FLearningAgentsObservationObjectElement Car = ULearningAgentsObservations::MakeVelocityObservation(InObservationObject,ObsActor->GetVelocity(),ObsActor->GetTransform(),200.0);

	Elements.Empty();

	Elements.Add(TEXT("Track"),Track);
	Elements.Add(TEXT("Car"),Car);

	OutObservationObjectElement = ULearningAgentsObservations::MakeStructObservation(InObservationObject,Elements);
}

void USportsCarInteractor::SpecifyAgentAction_Implementation(FLearningAgentsActionSchemaElement& OutActionSchemaElement,
	ULearningAgentsActionSchema* InActionSchema)
{
	FLearningAgentsActionSchemaElement Steering =  ULearningAgentsActions::SpecifyFloatAction(InActionSchema,TEXT("Steering"));
	FLearningAgentsActionSchemaElement ThrottleBrake = ULearningAgentsActions::SpecifyFloatAction(InActionSchema,TEXT("ThrottleBrake"));

	TMap<FName,FLearningAgentsActionSchemaElement> Elements;
	Elements.Add(TEXT("Steering"),Steering);
	Elements.Add(TEXT("ThrottleBrake"),ThrottleBrake);

	OutActionSchemaElement = ULearningAgentsActions::SpecifyStructAction(InActionSchema,Elements);
}

void USportsCarInteractor::PerformAgentAction_Implementation(const ULearningAgentsActionObject* InActionObject,
	const FLearningAgentsActionObjectElement& InActionObjectElement, const int32 AgentId)
{
	UObject* AgentObject = GetAgent(AgentId,ATest_TestSportsCar::StaticClass());
	check(AgentObject);
	ATest_TestSportsCar* ActActor = Cast<ATest_TestSportsCar>(AgentObject);
	TMap<FName, FLearningAgentsActionObjectElement> OutElements;


	//	static bool GetStructAction(TMap<FName, FLearningAgentsActionObjectElement>& OutElements, const ULearningAgentsActionObject* Object, const FLearningAgentsActionObjectElement Element, const FName Tag = TEXT("StructAction"));
	ULearningAgentsActions::GetStructAction(OutElements,InActionObject,InActionObjectElement);
	auto VehicleMovement = ActActor->GetVehicleMovement();
	
	//前进部分
	auto Steering =  *OutElements.Find(TEXT("Steering"));
	float OutValue;
	ULearningAgentsActions::GetFloatAction(OutValue,InActionObject,Steering,1,TEXT("Steering"),true);
	VehicleMovement->SetSteeringInput(OutValue);

	//刹车部分
	auto ThrottleBrake = *OutElements.Find(TEXT("ThrottleBrake"));
	ULearningAgentsActions::GetFloatAction(OutValue,InActionObject,ThrottleBrake,1,TEXT("ThrottleBrake"),true);
	
	if(OutValue > 0)
	{
		VehicleMovement->SetThrottleInput(OutValue);
		VehicleMovement->SetBrakeInput(0.0);
	}
	else
	{
		VehicleMovement->SetBrakeInput(OutValue);
		VehicleMovement->SetThrottleInput(0.0);
	}
	
}
