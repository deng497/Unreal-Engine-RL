// Fill out your copyright notice in the Description page of Project Settings.


#include "SportsCarTrainer.h"

#include "SportCarTrackSpline.h"
#include "Components/SplineComponent.h"
#include "Test_Test/Test_TestSportsCar.h"


USportsCarTrainer::USportsCarTrainer()
{
	//TrackSpline = CreateDefaultSubobject<USplineComponent>("TrackSpline");
}

void USportsCarTrainer::GatherAgentReward_Implementation(float& OutReward, const int32 AgentId)
{
	// 获取代理对象
	UObject* AgentObject = GetAgent(AgentId, ATest_TestSportsCar::StaticClass());
	check(AgentObject);
	ATest_TestSportsCar* RewardActor = Cast<ATest_TestSportsCar>(AgentObject);
    
	// 确保 TrackSpline 已经初始化
	check(TrackSpline);

	// 获取代理车辆的位置和速度
	FVector ActorLocation = RewardActor->GetActorLocation();
	FVector ActorVelocity = RewardActor->GetVelocity();

	// 计算基于速度的奖励
	float VelocityReward = ULearningAgentsRewards::MakeRewardFromVelocityAlongSpline(
		TrackSpline,                 // 样条曲线
		ActorLocation,               // 车辆位置
		ActorVelocity,               // 车辆速度
		1000.0f,                     // 速度标度（Velocity Scale）
		1.0f,                        // 奖励标度（Reward Scale）
		10.0f                        // 平滑值（Fillet Difference Delta）
	);

	// 找到样条曲线上距离车辆最近的点
	FVector ClosestPointOnSpline = TrackSpline->FindLocationClosestToWorldLocation(
		ActorLocation,               // 车辆的世界位置
		ESplineCoordinateSpace::World // 使用世界坐标系
	);

	// 计算基于位置差异的奖励（如果车辆与样条曲线距离大于某个阈值，则惩罚）
	float LocationReward = ULearningAgentsRewards::MakeRewardOnLocationDifferenceAboveThreshold(
		ActorLocation,               // 车辆的位置
		ClosestPointOnSpline,        // 样条曲线上距离车辆最近的点
		800.0f,                      // 距离阈值
		-10.0f                       // 奖励标度（负值表示惩罚）
	);

	// 最终奖励是速度奖励和位置奖励的加和
	OutReward = VelocityReward + LocationReward;

	// 可以取消注释以进行调试日志输出
	 //UE_LOG(LogTemp, Log, TEXT("Agent %d: VelocityReward = %f, LocationReward = %f, TotalReward = %f"), AgentId, VelocityReward, LocationReward, OutReward);
}

void USportsCarTrainer::GatherAgentCompletion_Implementation(ELearningAgentsCompletion& OutCompletion, const int32 AgentId)
{
	UObject* AgentObject = GetAgent(AgentId,ATest_TestSportsCar::StaticClass());
	check(AgentObject);
	ATest_TestSportsCar* CarActor = Cast<ATest_TestSportsCar>(AgentObject);

	auto TargetLocation =  TrackSpline->FindLocationClosestToWorldLocation(CarActor->GetActorLocation(),ESplineCoordinateSpace::World);

	OutCompletion = ULearningAgentsCompletions::MakeCompletionOnLocationDifferenceAboveThreshold(TargetLocation,CarActor->GetActorLocation(),800.0,ELearningAgentsCompletion::Termination);
}

void USportsCarTrainer::ResetAgentEpisode_Implementation(const int32 AgentId)
{
	UObject* AgentObject = GetAgent(AgentId,ATest_TestSportsCar::StaticClass());
	check(AgentObject);
	ATest_TestSportsCar* CarActor = Cast<ATest_TestSportsCar>(AgentObject);
	CarActor->ResetToRandomPointOnSpline(TrackSpline);
}


