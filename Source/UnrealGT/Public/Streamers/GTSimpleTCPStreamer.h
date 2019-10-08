// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GTDataStreamerComponent.h"

#include "GTSimpleTCPStreamer.generated.h"

class FGTThreadedTCPStreamer;

/**
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent),
       hidecategories = (Collision, Object, Physics, SceneComponent))
class UNREALGT_API UGTSimpleTCPStreamer : public UGTDataStreamerComponent {
  GENERATED_BODY()

public:
  UGTSimpleTCPStreamer();

  virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

protected:
  virtual void BeginPlay() override;

  virtual void OnDataReady(const TArray<uint8> &Data,
                           const FDateTime &TimeStamp) override;

private:
  UPROPERTY(EditAnywhere)
  FString IPAddress;

  UPROPERTY(EditAnywhere)
  uint32 Port;

  FGTThreadedTCPStreamer *TCPStreamer;
};
