// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LearningAgentsManager.h"
#include "LearningAgentsNeuralNetwork.h"
#include "LearningAgentsPolicy.h"
#include "SportCarTrackSpline.h"
#include "SportsCarTrainer.h"
#include "Components/SplineComponent.h"
#include "SportsCarManager.generated.h"

UCLASS()
class TEST_TEST_API ASportsCarManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASportsCarManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ULearningAgentsManager* LearningAgentsManager;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USplineComponent* Spline;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ASportCarTrackSpline* SportCarTrackSpline;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ULearningAgentsNeuralNetwork* EncoderNeuralNetworkAsset;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ULearningAgentsNeuralNetwork* PolicyNeuralNetworkAsset;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ULearningAgentsNeuralNetwork* DecoderNeuralNetworkAsset;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ULearningAgentsNeuralNetwork* CriticNeuralNetworkAsset;
public:
	bool RunInterface = true;
	
	UPROPERTY(VisibleAnywhere)
	ULearningAgentsPolicy* Policy;
	
	UPROPERTY(VisibleAnywhere)
	USportsCarTrainer* CarTrainer;
	
};
