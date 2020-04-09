// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Async/Async.h"
#include "Async/AsyncWork.h"
#include "CoreMinimal.h"
#include "Misc/FileHelper.h"

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

/**
 *
 */
class UNREALGT_API FGTFileUtilities
{
public:
    static const FDateTime SessionStartTime;
    static const TCHAR* TimeFormat;

    static TArray<uint8> StringToCharArray(const FString& InString);

    static void
    WriteFileToSessionDirectory(FString FileName, const TArray<uint8>& Data, UWorld* CurrentWorld);
};
