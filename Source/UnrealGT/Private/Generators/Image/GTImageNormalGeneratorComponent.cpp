// Fill out your copyright notice in the Description page of Project Settings.

#include "GTImageNormalGeneratorComponent.h"

#include "GTSceneCaptureComponent2D.h"
#include "GTImageGeneratorBase.h"

UGTImageNormalGeneratorComponent::UGTImageNormalGeneratorComponent()
    : Super()
{
    SceneCaptureComponent->SetupWorldNormalPostProccess();
}

