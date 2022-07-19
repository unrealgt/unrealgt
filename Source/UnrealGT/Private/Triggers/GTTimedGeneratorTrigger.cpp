// Fill out your copyright notice in the Description page of Project Settings.

#include "Triggers/GTTimedGeneratorTrigger.h"

#include <Engine/World.h>
#include <TimerManager.h>

#include "Generators/GTDataGeneratorComponent.h"

// Sets default values for this component's properties
UGTTimedGeneratorTrigger::UGTTimedGeneratorTrigger()
    : bTriggerEveryFrame(false)
    , FrameRate(24.f)
{
}

// Called when the game starts
void UGTTimedGeneratorTrigger::BeginPlay()
{
    Super::BeginPlay();

    if (FrameRate <= 0.F)
    {
        GetWorld()->GetTimerManager().SetTimer(
            TriggerTimerHandle, this, &UGTTimedGeneratorTrigger::Trigger, 1 / FrameRate, true);
    }
}