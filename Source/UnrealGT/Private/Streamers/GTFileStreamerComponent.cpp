// Fill out your copyright notice in the Description page of Project Settings.

#include "Streamers/GTFileStreamerComponent.h"

#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// TODO move to extra file
class FGTSaveFileTask : public FNonAbandonableTask
{
    friend class FAutoDeleteAsyncTask<FGTSaveFileTask>;

public:
    FGTSaveFileTask(const FString& TotalFileName, const TArray<uint8>& Data)
        : TotalFileName(TotalFileName)
        , Data(Data)
    {
    }

protected:
    FString TotalFileName;
    TArray<uint8> Data;

    void DoWork()
    {
        FFileHelper::SaveArrayToFile(Data, *TotalFileName);
    }

    FORCEINLINE TStatId GetStatId() const
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(FGTSaveFileTask, STATGROUP_ThreadPoolAsyncTasks);
    }
};

const FDateTime UGTFileStreamerComponent::SessionStartTime = FDateTime::Now();

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
    const TCHAR* TimeFormat = TEXT("%Y%m%dT%H%M%S%sZ");

    FString SessionDir = FPaths::ConvertRelativePathToFull(FPaths::Combine(
        FPaths::ProjectSavedDir(),
        TEXT("UnrealGT"),
        GetWorld()->GetMapName(),
        SessionStartTime.ToString(TimeFormat),
        GetName()));

    FString TimeString = TimeStamp.ToString(TimeFormat);

    TMap<FString, FStringFormatArg> GlobalProperties{{TEXT("ID"), IDCounter},
                                                     {TEXT("Time"), TimeString}};

    IDCounter++;

    FString FileName = FString::Format(*FileNameFormat, GlobalProperties);

    FString TotalFileName = FPaths::Combine(*SessionDir, *FileName);

    (new FAutoDeleteAsyncTask<FGTSaveFileTask>(TotalFileName, Data))->StartBackgroundTask();
}
