// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GTImage.h"
#include "GTImageFileFormat.h"
#include <AsyncWork.h>

class UGTImageGeneratorBase;

/**
 *
 */
class UNREALGT_API FGTAsyncMakeImageTask : public FNonAbandonableTask {
  friend class FAutoDeleteAsyncTask<FGTAsyncMakeImageTask>;

public:
  FGTAsyncMakeImageTask(UGTImageGeneratorBase *SourceComponent,
                        const FGTImage &Image, EGTImageFileFormat ImageFormat,
                        bool bWriteAlpha, FDateTime TimeStamp);

  void DoWork();

  TStatId GetStatId() const;

private:
  UGTImageGeneratorBase *SourceComponent;
  FGTImage Image;
  EGTImageFileFormat ImageFormat;
  FDateTime TimeStamp;
  bool bWriteAlpha;
};
