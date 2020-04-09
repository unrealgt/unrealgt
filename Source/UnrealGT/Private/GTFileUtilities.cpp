// Fill out your copyright notice in the Description page of Project Settings.

#include "GTFileUtilities.h"

#include "Engine/World.h"
#include "Misc/Paths.h"

const FDateTime FGTFileUtilities::SessionStartTime = FDateTime::Now();
const TCHAR* FGTFileUtilities::TimeFormat = TEXT("%Y%m%dT%H%M%S%sZ");

TArray<uint8> FGTFileUtilities::StringToCharArray(const FString& InString)
{
    TArray<uint8> Data;
    Data.Reserve(InString.Len());

    FTCHARToUTF8 UTF8Str(*InString);

    for (int I = 0; I < UTF8Str.Length(); I++)
    {
        Data.Add(UTF8Str.Get()[I]);
    }

    return Data;
}

void FGTFileUtilities::WriteFileToSessionDirectory(
    FString FileName,
    const TArray<uint8>& Data,
    UWorld* CurrentWorld)
{
    FString SessionDir = FPaths::ConvertRelativePathToFull(FPaths::Combine(
        FPaths::ProjectSavedDir(),
        TEXT("UnrealGT"),
        CurrentWorld->GetMapName(),
        SessionStartTime.ToString(TimeFormat)));

    FString TotalFileName = FPaths::Combine(SessionDir, FileName);

    (new FAutoDeleteAsyncTask<FGTSaveFileTask>(TotalFileName, Data))->StartBackgroundTask();
}
