// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GTDataStreamerComponent.h"

#include "GTFileStreamerComponent.generated.h"

/**
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent),
       hidecategories = (Collision, Object, Physics, SceneComponent))
class UNREALGT_API UGTFileStreamerComponent : public UGTDataStreamerComponent {
  GENERATED_BODY()
public:
  UGTFileStreamerComponent();

protected:
  virtual void BeginPlay() override;

  virtual void OnDataReady(const TArray<uint8> &Data,
                           const FDateTime &TimeStamp) override;

private:
  UPROPERTY(EditAnywhere)
  FString FileNameFormat;

  int IDCounter;

  static const FDateTime SessionStartTime;
};
