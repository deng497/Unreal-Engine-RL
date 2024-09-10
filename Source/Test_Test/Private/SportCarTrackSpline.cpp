// Fill out your copyright notice in the Description page of Project Settings.


#include "SportCarTrackSpline.h"

// Sets default values
ASportCarTrackSpline::ASportCarTrackSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>("TrackSpline");
	SplineComponent->SetupAttachment(RootComponent);

	SplineComponent->SetClosedLoop(true,true);
	SplineComponent->SetDrawDebug(true);
}

// Called when the game starts or when spawned
void ASportCarTrackSpline::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASportCarTrackSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

