# 赛车强化学习系统技术文档

## 目录

- [赛车强化学习系统技术文档](#赛车强化学习系统技术文档)
  - [目录](#目录)
  - [概述](#概述)
  - [系统架构](#系统架构)
    - [系统工作流程：](#系统工作流程)
  - [核心类介绍](#核心类介绍)
    - [1. `ASportsCarManager`](#1-asportscarmanager)
      - [关键功能：](#关键功能)
    - [2. `USportsCarInteractor`](#2-usportscarinteractor)
      - [关键功能：](#关键功能-1)
    - [3. `USportsCarTrainer`](#3-usportscartrainer)
      - [关键功能：](#关键功能-2)
    - [4. `ATest_TestSportsCar`](#4-atest_testsportscar)
      - [关键功能：](#关键功能-3)
  - [功能流程](#功能流程)
  - [奖励机制](#奖励机制)
  - [车辆重置机制](#车辆重置机制)
    - [重置流程：](#重置流程)
  - [总结](#总结)

## 概述

本项目旨在实现一个基于强化学习的赛车系统，通过强化学习让AI代理（赛车）学会在赛道上驾驶。系统使用`Unreal Engine`框架，基于`Learning Agents Framework`构建，包含策略、批评者和训练器。代理在赛道上通过观察、决策和执行动作来学习如何驾驶，并根据表现获得奖励。

## 系统架构

系统主要包括以下几个模块：

- **代理（Agent）**：每个赛车对象通过`ATest_TestSportsCar`类表示。代理在环境中与赛道进行交互，获取信息并执行动作。
- **交互系统（Interactor）**：由`USportsCarInteractor`类实现，用于获取赛车的观察数据（如位置、速度）并执行策略生成的动作。
- **强化学习管理器（Manager）**：由`ASportsCarManager`类实现，管理多个赛车代理的生命周期，包括代理初始化、训练及推理等。
- **训练器（Trainer）**：由`USportsCarTrainer`类实现，负责执行强化学习的训练逻辑，计算奖励及判断回合结束。
  
### 系统工作流程：

1. **观察收集**：代理（赛车）通过交互系统收集其在赛道中的位置、速度等信息。
2. **策略决策**：根据观察信息，策略生成代理的下一步动作。
3. **动作执行**：代理根据策略生成的动作调整其在赛道中的行为，如加速、转向等。
4. **奖励与反馈**：系统根据代理的表现（如行驶速度、与赛道的距离等）给予奖励或惩罚。
5. **学习与改进**：代理通过多轮训练，逐步优化其驾驶行为。

## 核心类介绍

### 1. `ASportsCarManager`

`ASportsCarManager`是整个系统的管理者，负责初始化代理（赛车）、创建强化学习的核心组件并执行训练或推理任务。它的职责包括：

1. **初始化代理**：系统启动时，管理器负责初始化所有赛车，并设置Tick的依赖关系，确保管理器在代理之前被调用。
2. **强化学习组件的创建**：包括创建**策略**、**批评者**和**训练器**，用于执行强化学习任务。
3. **训练与推理**：根据运行模式（训练或推理），管理器负责调用相应的逻辑。

```cpp
ASportsCarManager::ASportsCarManager()
{
    PrimaryActorTick.bCanEverTick = true;  // 设置每帧调用Tick函数

    // 创建LearningAgentsManager，用于管理强化学习代理
    LearningAgentsManager = CreateDefaultSubobject<ULearningAgentsManager>("LearningAgentsManager");

    // 设置标签，便于在场景中查找
    Tags.Add("LearningAgentsManager");
}
```

#### 关键功能：

- **BeginPlay**：在游戏开始时，初始化代理和强化学习组件。
- **Tick**：每帧调用，负责执行推理或训练任务。
  
在`BeginPlay`中，管理器会获取场景中的所有赛车代理并初始化相关组件（策略、批评者、训练器）：

```cpp
void ASportsCarManager::BeginPlay()
{
    Super::BeginPlay();
    
    // 初始化赛车
    TArray<AActor*> CarActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATest_TestSportsCar::StaticClass(), CarActors);

    // 初始化样条曲线（赛道）
    AActor* SplineActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASportCarTrackSpline::StaticClass());
    if (SplineActor)
    {
        SportCarTrackSpline = Cast<ASportCarTrackSpline>(SplineActor);
        Spline = SportCarTrackSpline->SplineComponent;
    }

    // 初始化强化学习的Interactor，设置轨迹样条
    USportsCarInteractor* Interactor = Cast<USportsCarInteractor>(ULearningAgentsInteractor::MakeInteractor(LearningAgentsManager, USportsCarInteractor::StaticClass(), TEXT("SportsCarInteractor")));
    if (Interactor)
    {
        Interactor->TrackSpline = Spline;
    }

    // 创建策略对象
    FLearningAgentsPolicySettings PolicySettings;
    Policy = ULearningAgentsPolicy::MakePolicy(LearningAgentsManager, Interactor, ULearningAgentsPolicy::StaticClass(), TEXT("SportsCarPolicy"), EncoderNeuralNetworkAsset, PolicyNeuralNetworkAsset, DecoderNeuralNetworkAsset, !RunInterface, !RunInterface, !RunInterface, PolicySettings);

    // 创建批评者对象
    FLearningAgentsCriticSettings CriticSettings;
    ULearningAgentsCritic* Critic = ULearningAgentsCritic::MakeCritic(LearningAgentsManager, Interactor, Policy, ULearningAgentsCritic::StaticClass(), TEXT("SportsCarCritic"), CriticNeuralNetworkAsset, !RunInterface, CriticSettings);

    // 创建训练器对象
    FLearningAgentsTrainerSettings TrainerSettings;
    ULearningAgentsTrainer* Trainer = ULearningAgentsTrainer::MakeTrainer(LearningAgentsManager, Interactor, Policy, Critic, USportsCarTrainer::StaticClass(), TEXT("SportsCarTrainer"), TrainerSettings);

    // 车辆位置初始化
    if (RunInterface)
    {
        for (auto Cars : CarActors)
        {
            ATest_TestSportsCar* SportsCar = Cast<ATest_TestSportsCar>(Cars);
            if (SportsCar)
            {
                SportsCar->ResetToRandomPointOnSpline(Spline);
            }
        }
    }
}
```

### 2. `USportsCarInteractor`

`USportsCarInteractor`类用于管理赛车与环境之间的交互，主要负责：

1. **收集赛车的观察信息**：如赛车在赛道上的位置、方向和速度。
2. **执行策略动作**：根据策略生成的动作来控制赛车的行为。
3. **将执行后的结果反馈给训练系统**。

#### 关键功能：

- **SpecifyAgentObservation**：收集赛车的观察信息，包括在赛道上的位置、速度等。
- **SpecifyAgentAction**：执行策略生成的动作，如控制赛车的方向和速度。
- **GatherAgentObservation**：实时获取赛车的运动状态并更新到训练器中。

```cpp
void USportsCarInteractor::SpecifyAgentObservation_Implementation(FLearningAgentsObservationSchemaElement& OutObservationSchemaElement, ULearningAgentsObservationSchema* InObservationSchema)
{
    // 采集赛车的位置、方向和速度
    FLearningAgentsObservationSchemaElement LocationObservation = ULearningAgentsObservations::SpecifyLocationAlongSplineObservation(InObservationSchema);
    FLearningAgentsObservationSchemaElement DirectionObservation = ULearningAgentsObservations::SpecifyDirectionAlongSplineObservation(InObservationSchema);
    FLearningAgentsObservationSchemaElement VelocityObservation = ULearningAgentsObservations::SpecifyVelocityObservation(InObservationSchema);

    // 将这些观察信息封装成结构体并返回
    TMap<FName, FLearningAgentsObservationSchemaElement> Elements;
    Elements.Add(TEXT("Location"), LocationObservation);
    Elements.Add(TEXT("Direction"), DirectionObservation);
    Elements.Add(TEXT("Car"), VelocityObservation);
    OutObservationSchemaElement = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, Elements);
}
```

### 3. `USportsCarTrainer`

`USportsCarTrainer`是强化学习中的训练器类，主要用于：

1. **计算奖励**：根据赛车的表现计算奖励，鼓励代理沿着赛道行驶，惩罚偏离赛道的行为。
2. **检查终止条件**：当某些条件满足时，训练器会判断是否结束当前回合并重置代理。

#### 关键功能：

- **GatherAgentReward**：计算并返回基于速度和位置的奖励。
- **ResetAgentEpisode**：在回合结束时，重置赛车到赛道的随机位置。

```cpp
void USportsCarTrainer::GatherAgentReward_Implementation(float& OutReward, const int32 AgentId)
{
    // 获取赛车的位置和速度
    FVector ActorLocation = RewardActor->GetActorLocation();
    FVector ActorVelocity = RewardActor->GetVelocity();

    // 计算速度奖励，速度越快奖励越高
    float VelocityReward = ULearningAgentsRewards::MakeRewardFromVelocityAlongSpline(TrackSpline, ActorLocation, ActorVelocity, 1000.0f, 1.0f, 10.0f);

    // 计算位置奖励，离赛道越近奖励越高
    FVector ClosestPointOnSpline = TrackSpline->FindLocationClosestToWorldLocation(ActorLocation, ESplineCoordinateSpace::World);
    float LocationReward = ULearningAgentsRewards::MakeRewardOnLocationDifferenceAboveThreshold(ActorLocation, ClosestPointOnSpline,

 800.0f, -10.0f);

    OutReward = VelocityReward + LocationReward;
}
```

### 4. `ATest_TestSportsCar`

`ATest_TestSportsCar`是具体的赛车代理类，继承自引擎的车辆类，包含车辆的物理属性和与赛道的交互逻辑。

#### 关键功能：

- **ResetToRandomPointOnSpline**：将赛车重置到赛道上的随机位置，确保与其他赛车的距离符合要求。

```cpp
void ATest_TestSportsCar::ResetToRandomPointOnSpline(USplineComponent* Spline)
{
    FTransform CandidateTransform;
    bool bValidTransform = false;

    // 随机生成有效的候选位置
    while (!bValidTransform)
    {
        // 随机生成偏移量
        float RandX = (UKismetMathLibrary::RandomFloat() - 0.5f) * 1200.0f;
        float RandY = (UKismetMathLibrary::RandomFloat() - 0.5f) * 1200.0f;
        FVector RandOffset = FVector(RandX, RandY, 50.0f);

        // 在样条曲线上随机选择位置
        float RandSplineDistance = Spline->GetSplineLength() * UKismetMathLibrary::RandomFloat();
        FVector RandLocationOnSpline = Spline->GetLocationAtDistanceAlongSpline(RandSplineDistance, ESplineCoordinateSpace::World);
        FVector FinalLocation = RandLocationOnSpline + RandOffset;

        // 获取样条曲线的旋转
        FRotator RandRotator = Spline->GetRotationAtDistanceAlongSpline(RandSplineDistance, ESplineCoordinateSpace::World);
        RandRotator.Yaw += (UKismetMathLibrary::RandomFloat() - 0.5f) * 90.0f;

        CandidateTransform = FTransform(RandRotator, FinalLocation, FVector(1.0f, 1.0f, 1.0f));

        // 检查距离其他车辆是否足够远
        bValidTransform = true;
        TArray<AActor*> OutActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATest_TestSportsCar::StaticClass(), OutActors);
        for (AActor* OtherActor : OutActors)
        {
            if (FVector::Dist(CandidateTransform.GetLocation(), OtherActor->GetActorLocation()) < 1500.0f)
            {
                bValidTransform = false;
                break;
            }
        }
    }

    // 将车辆传送到新的位置
    SetActorTransform(CandidateTransform, false, nullptr, ETeleportType::TeleportPhysics);

    // 重置车辆的物理属性
    GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
    GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
}
```

## 功能流程

1. **代理的观察获取**：通过`USportsCarInteractor`采集代理的观察信息，包括位置、速度、方向等。观察信息通过`SpecifyAgentObservation`函数传递给强化学习框架。
2. **策略的动作生成**：`ULearningAgentsPolicy`根据观察信息生成动作，具体动作包括转向、加速或刹车。
3. **动作执行**：代理根据生成的动作调整其在赛道中的运动状态，通过物理系统控制赛车行为。
4. **奖励计算**：`USportsCarTrainer`根据赛车的表现计算奖励，奖励与赛车的速度和赛道位置相关。系统通过`GatherAgentReward`函数计算奖励值。
5. **回合终止与重置**：当训练回合结束时，系统调用`ResetToRandomPointOnSpline`函数，将赛车重置到赛道的随机位置。

## 奖励机制

奖励机制用于引导代理优化其行为，主要基于以下两个因素：

1. **速度奖励**：鼓励代理尽可能快地沿着赛道前进。代理的速度越高，奖励越高。奖励通过`MakeRewardFromVelocityAlongSpline`计算。
2. **位置奖励**：奖励代理保持与赛道中心线的距离越小越好，偏离赛道会受到惩罚。位置奖励通过`MakeRewardOnLocationDifferenceAboveThreshold`计算，超过阈值则给予惩罚。

```cpp
float VelocityReward = ULearningAgentsRewards::MakeRewardFromVelocityAlongSpline(TrackSpline, ActorLocation, ActorVelocity, 1000.0f, 1.0f, 10.0f);
float LocationReward = ULearningAgentsRewards::MakeRewardOnLocationDifferenceAboveThreshold(ActorLocation, ClosestPointOnSpline, 800.0f, -10.0f);
```

## 车辆重置机制

车辆在训练回合结束时会被重置到赛道的随机位置。通过`ResetToRandomPointOnSpline`函数实现重置，确保车辆重置后不会与其他车辆过于接近。

### 重置流程：

1. 通过样条曲线生成随机位置，确保位置符合赛道要求。
2. 计算与其他车辆的距离，确保安全距离不小于1500单位。
3. 重置车辆的位置和物理状态，使其恢复到初始状态。

```cpp
SetActorTransform(CandidateTransform, false, nullptr, ETeleportType::TeleportPhysics);
GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
```

## 总结

通过这一系统，赛车代理可以逐步学习如何在赛道上行驶，并通过强化学习算法优化其驾驶策略。系统通过观察、动作、奖励和反馈回路不断迭代，提高代理的表现。