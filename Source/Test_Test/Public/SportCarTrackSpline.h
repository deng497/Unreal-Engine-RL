// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "SportCarTrackSpline.generated.h"

UCLASS()
class TEST_TEST_API ASportCarTrackSpline : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	USplineComponent* SplineComponent;
public:	
	// Sets default values for this actor's properties
	ASportCarTrackSpline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
