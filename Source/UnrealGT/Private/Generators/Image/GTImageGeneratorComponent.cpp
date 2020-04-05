// Fill out your copyright notice in the Description page of Project Settings.

#include "Generators/Image/GTImageGeneratorComponent.h"

#include "Generators/Image/GTSceneCaptureComponent2D.h"

UGTImageGeneratorComponent::UGTImageGeneratorComponent()
    : Super()
    , bUseDisplayGamma(true)
    , TargetGamma(1.4)
    , bUseRandomGamma(false)
    , TargetGammaMax(2.4)
{
    OrthoWidth = 512;
    ClipPlaneNormal = FVector(0, 0, 1);
}

void UGTImageGeneratorComponent::GenerateData(const FDateTime& TimeStamp)
{
    if (bUseRandomGamma)
    {
        SceneCaptureComponent->TargetGamma = FMath::FRandRange(TargetGamma, TargetGammaMax);
    }

    Super::GenerateData(TimeStamp);
}

void UGTImageGeneratorComponent::BeginPlay()
{
    Super::BeginPlay();

    SceneCaptureComponent->bUseDisplayGamma = bUseDisplayGamma;
    SceneCaptureComponent->TargetGamma = TargetGamma;
    SceneCaptureComponent->ProjectionType = ProjectionType;
    SceneCaptureComponent->OrthoWidth = OrthoWidth;
    SceneCaptureComponent->PostProcessSettings = PostProcessSettings;
    SceneCaptureComponent->bEnableClipPlane = bEnableClipPlane;
    SceneCaptureComponent->ClipPlaneBase = ClipPlaneBase;
    SceneCaptureComponent->ClipPlaneNormal = ClipPlaneNormal;
}
