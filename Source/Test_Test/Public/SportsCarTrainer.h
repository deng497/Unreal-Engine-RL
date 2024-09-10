// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsTrainer.h"
#include "LearningAgentsRewards.h"
#include "SportsCarInteractor.h"
#include "SportsCarTrainer.generated.h"

/**
 * 
 */
UCLASS()
class TEST_TEST_API USportsCarTrainer : public ULearningAgentsTrainer
{
	GENERATED_BODY()
public:
	USportsCarTrainer();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USplineComponent* TrackSpline;

	void GatherAgentReward_Implementation(float& OutReward, const int32 AgentId);
	void GatherAgentCompletion_Implementation(ELearningAgentsCompletion& OutCompletion, const int32 AgentId);
	void ResetAgentEpisode_Implementation(const int32 AgentId);

	
};
