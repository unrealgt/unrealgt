// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GTDataStreamerComponent.h"

#include "GTHttpStreamerComponent.generated.h"

UENUM(BlueprintType)
enum class EGTHttpMethod : uint8
{
    Post,
    Put,
};

UENUM(BlueprintType)
enum class EGTHttpContentType : uint8
{
    Json,
    PNG,
    BMP,
    TextPlain,
    OctetStream
};

/**
 * Example: https://github.com/unrealgt/unrealgt/tree/master/Examples/PythonHTTPClient
 */
UCLASS(
    ClassGroup = (Custom),
    meta = (BlueprintSpawnableComponent),
    hidecategories = (Collision, Object, Physics, SceneComponent))
class UNREALGT_API UGTHttpStreamerComponent : public UGTDataStreamerComponent
{
    GENERATED_BODY()

public:
    UGTHttpStreamerComponent();

protected:
    virtual void OnDataReady(const TArray<uint8>& Data, const FDateTime& TimeStamp) override;

private:
    UPROPERTY(EditDefaultsOnly, Category = Http)
    FString TargetUrl;

    UPROPERTY(EditDefaultsOnly, Category = Http)
    EGTHttpMethod Method;

    UPROPERTY(EditDefaultsOnly, Category = Http)
    EGTHttpContentType ContentType;
};
