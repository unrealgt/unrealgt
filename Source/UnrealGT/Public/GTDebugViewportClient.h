// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealClient.h"

class UGTDataGeneratorComponent;

/**
 *
 */
class UNREALGT_API FGTDebugViewportClient : public FViewportClient {
public:
  virtual void Draw(FViewport *Viewport, FCanvas *Canvas) override;

  void SetDataGenerator(UGTDataGeneratorComponent *DataGenerator);

private:
  TWeakObjectPtr<UGTDataGeneratorComponent> CurrentGenerator;
};
