// Fill out your copyright notice in the Description page of Project Settings.

#include "Streamers/GTFileStreamerComponent.h"

#include "Engine/World.h"
#include "Misc/Paths.h"

#include "GTFileUtilities.h"

UGTFileStreamerComponent::UGTFileStreamerComponent()
    : FileNameFormat(TEXT("{ID}_{Time}.txt"))
    , IDCounter(0)
{
}

void UGTFileStreamerComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UGTFileStreamerComponent::OnDataReady(const TArray<uint8>& Data, const FDateTime& TimeStamp)
{
    FString TimeString = TimeStamp.ToString(FGTFileUtilities::TimeFormat);

    TMap<FString, FStringFormatArg> GlobalProperties{{TEXT("ID"), IDCounter},
                                                     {TEXT("Time"), TimeString}};

    IDCounter++;

    FString FileName = FString::Format(*FileNameFormat, GlobalProperties);

    FString TotalFileName = FPaths::Combine(GetName(), FileName);

    FGTFileUtilities::WriteFileToSessionDirectory(TotalFileName, Data, GetWorld());
}
