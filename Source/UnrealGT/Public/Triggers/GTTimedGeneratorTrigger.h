// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GTGeneratorTrigger.h"
#include "CoreMinimal.h"

#include "GTGeneratorReference.h"

#include "GTTimedGeneratorTrigger.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALGT_API UGTTimedGeneratorTrigger : public UGTGeneratorTrigger
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UGTTimedGeneratorTrigger();

protected:
  // Called when the game starts
  virtual void BeginPlay() override;

private:
  UPROPERTY(EditAnywhere, Category = TriggerSettings)
  bool bTriggerEveryFrame;

  UPROPERTY(EditAnywhere, Category = TriggerSettings,
            meta = (EditCondition = "!bTriggerEveryFrame", ClampMin = 0))
  float FrameRate;

  FTimerHandle TriggerTimerHandle;
};
