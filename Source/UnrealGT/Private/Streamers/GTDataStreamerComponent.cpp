// Fill out your copyright notice in the Description page of Project Settings.

#include "Streamers/GTDataStreamerComponent.h"

#include "Generators/GTDataGeneratorComponent.h"

// Sets default values for this component's properties
UGTDataStreamerComponent::UGTDataStreamerComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked
    // every frame.  You can turn these features off to improve performance if you
    // don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGTDataStreamerComponent::BeginPlay()
{
    Super::BeginPlay();

    UGTDataGeneratorComponent* Generator = GeneratorReference.GetComponent(GetOwner());

    if (Generator)
    {
        Generator->DataReadyDelegate.AddDynamic(this, &UGTDataStreamerComponent::OnDataReady);
    }
}

void UGTDataStreamerComponent::OnDataReady(const TArray<uint8>& Data, const FDateTime& TimeStamp)
{
}

// Called every frame
void UGTDataStreamerComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}
