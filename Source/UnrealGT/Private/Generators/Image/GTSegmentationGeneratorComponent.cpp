// Fill out your copyright notice in the Description page of Project Settings.

#include "GTSegmentationGeneratorComponent.h"

#include "GTSceneCaptureComponent2D.h"

UGTSegmentationGeneratorComponent::UGTSegmentationGeneratorComponent()
    : Super()
{
}

void UGTSegmentationGeneratorComponent::BeginPlay()
{
    Super::BeginPlay();

    SceneCaptureComponent->SetupSegmentationPostProccess(ComponentToColor, false, bShouldApplyCloseMorph);
}
