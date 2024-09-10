#include "Test_Test/Public/SportsCarManager.h"
#include "LearningAgentsPolicy.h"
#include "LearningAgentsInteractor.h"
#include "SportCarTrackSpline.h"
#include "LearningAgentsCritic.h"
#include "DA_SportsCarEncoder.h"
#include "LearningAgentsTrainer.h"
#include "SportsCarCritic.h"
#include "SportsCarDecoder.h"
#include "SportsCarInteractor.h"
#include "SportsCarPolicy.h"
#include "SportsCarTrainer.h"
#include "Kismet/GameplayStatics.h"
#include "Test_Test/Test_TestSportsCar.h"

// Sets default values
ASportsCarManager::ASportsCarManager()
{
	// 设置此Actor每帧调用Tick()，如果不需要，可以关闭它来提高性能
	PrimaryActorTick.bCanEverTick = true;
	
	// 创建LearningAgentsManager对象，用于管理强化学习代理
	LearningAgentsManager = CreateDefaultSubobject<ULearningAgentsManager>("LearningAgentsManager");

	// 设置标签为 "LearningAgentsManager"，可以通过标签在场景中找到该对象
	Tags.Add("LearningAgentsManager");
}

// Called when the game starts or when spawned
void ASportsCarManager::BeginPlay()
{
	Super::BeginPlay();
	
	// 定义一个USportsCarInteractor对象，用于管理赛车和样条曲线的交互
	USportsCarInteractor* Interactor;

	// 获取所有ATest_TestSportsCar类的Actor，存储在CarActors数组中
	TArray<AActor*> CarActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATest_TestSportsCar::StaticClass(), CarActors);

	// 为每辆赛车设置Tick的依赖关系，确保SportsCarManager在这些赛车之前进行更新
	for (auto result : CarActors)
	{
		ATest_TestSportsCar* Cars = Cast<ATest_TestSportsCar>(result);
		Cars->AddTickPrerequisiteActor(this); // 设置依赖：SportsCarManager的Tick先于车辆的Tick
	}

	// 获取场景中的赛道样条Actor，并获取样条组件
	AActor* SplineActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASportCarTrackSpline::StaticClass());
	if (SplineActor)
	{
		// 确保样条存在并获取其SplineComponent
		SportCarTrackSpline = Cast<ASportCarTrackSpline>(SplineActor);
		Spline = SportCarTrackSpline->SplineComponent;
	}

	// 创建Interactor实例，并将赛道样条赋值给它，用于交互
	Interactor = Cast<USportsCarInteractor>(ULearningAgentsInteractor::MakeInteractor(LearningAgentsManager, USportsCarInteractor::StaticClass(), TEXT("SportsCarInteractor")));
	if (Interactor)
	{
		Interactor->TrackSpline = Spline; // 为Interactor设置样条轨迹
	}

	// 创建策略对象，用于控制强化学习代理的决策
	FLearningAgentsPolicySettings PolicySettings;
	Policy = ULearningAgentsPolicy::MakePolicy(
		LearningAgentsManager,           // LearningAgents管理器
		Interactor,                      // Interactor实例
		ULearningAgentsPolicy::StaticClass(), // 策略类
		TEXT("SportsCarPolicy"),         // 策略的名称
		EncoderNeuralNetworkAsset,       // 编码器神经网络
		PolicyNeuralNetworkAsset,        // 策略神经网络
		DecoderNeuralNetworkAsset,       // 解码器神经网络
		false, false, false, // 参数用于控制界面设置
		PolicySettings                   // 策略的设置
	);

	// 创建批评者对象，用于强化学习中的价值评估
	FLearningAgentsCriticSettings CriticSettings;
	ULearningAgentsCritic* Critic = ULearningAgentsCritic::MakeCritic(
		LearningAgentsManager,            // LearningAgents管理器
		Interactor,                       // Interactor实例
		Policy,                           // 策略对象
		ULearningAgentsCritic::StaticClass(), // 批评者类
		TEXT("SportsCarCritic"),          // 批评者名称
		CriticNeuralNetworkAsset,         // 批评者神经网络
		!RunInterface,                    // 控制是否运行界面
		CriticSettings                    // 批评者设置
	);

	// 创建训练器对象，用于控制强化学习的训练过程
	FLearningAgentsTrainerSettings TrainerSettings;
	ULearningAgentsTrainer* Trainer = ULearningAgentsTrainer::MakeTrainer(
		LearningAgentsManager,             // LearningAgents管理器
		Interactor,                        // Interactor实例
		Policy,                            // 策略对象
		Critic,                            // 批评者对象
		USportsCarTrainer::StaticClass(),  // 训练器类
		TEXT("SportsCarTrainer"),          // 训练器名称
		TrainerSettings                    // 训练器设置
	);

	// 将Trainer转换为特定的赛车训练器对象，并设置轨迹样条
	CarTrainer = Cast<USportsCarTrainer>(Trainer);
	if (CarTrainer)
	{
		CarTrainer->TrackSpline = Spline; // 为训练器设置样条轨迹
	}

	// 如果RunInterface为真，重置每辆车的位置到赛道的随机点
	if (RunInterface)
	{
		for (auto Cars : CarActors)
		{
			ATest_TestSportsCar* SportsCar = Cast<ATest_TestSportsCar>(Cars);
			if (SportsCar)
			{
				SportsCar->ResetToRandomPointOnSpline(Spline); // 重置赛车到随机点
			}
		}
	}
}

// Called every frame
void ASportsCarManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!RunInterface)
	{
		// 如果运行的是推理模式，则执行策略的推理逻辑
		if (Policy)
		{
			Policy->RunInference(0.0); // 运行推理，产生动作
		}
	}
	else
	{
		// 如果运行的是训练模式，则执行训练逻辑
		if (CarTrainer)
		{
			FLearningAgentsTrainerTrainingSettings TrainerTrainingSettings;
			FLearningAgentsTrainerGameSettings TrainerGameSettings;
			FLearningAgentsTrainerPathSettings TrainerPathSettings;

			// 运行强化学习的训练过程
			CarTrainer->RunTraining(TrainerTrainingSettings, TrainerGameSettings, TrainerPathSettings);
		}
	}
}
