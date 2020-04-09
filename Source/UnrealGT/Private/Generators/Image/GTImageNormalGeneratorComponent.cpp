// Fill out your copyright notice in the Description page of Project Settings.

#include "Generators/Image/GTImageNormalGeneratorComponent.h"

#include "Generators/Image/GTSceneCaptureComponent2D.h"

UGTImageNormalGeneratorComponent::UGTImageNormalGeneratorComponent()
    : Super()
{
    bAntiAliasing = false;
    SceneCaptureComponent->SetupWorldNormalPostProccess();
}
