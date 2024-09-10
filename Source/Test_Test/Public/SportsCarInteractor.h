// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsInteractor.h"
#include "SportsCarInteractor.generated.h"

/**
 * 
 */
UCLASS()
class TEST_TEST_API USportsCarInteractor : public ULearningAgentsInteractor
{
	GENERATED_BODY()
private:
	USportsCarInteractor();
	
public:
	// 添加 SplineComponent 成员变量
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USplineComponent* TrackSpline;
public:
	void SpecifyAgentObservation_Implementation (FLearningAgentsObservationSchemaElement& OutObservationSchemaElement, ULearningAgentsObservationSchema* InObservationSchema);
	void GatherAgentObservation_Implementation(FLearningAgentsObservationObjectElement& OutObservationObjectElement, ULearningAgentsObservationObject* InObservationObject, const int32 AgentId);
	void SpecifyAgentAction_Implementation(FLearningAgentsActionSchemaElement& OutActionSchemaElement, ULearningAgentsActionSchema* InActionSchema);
	void PerformAgentAction_Implementation(const ULearningAgentsActionObject* InActionObject, const FLearningAgentsActionObjectElement& InActionObjectElement, const int32 AgentId);
};
