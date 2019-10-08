// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"
#include "CoreMinimal.h"

#include "GTDataGeneratorComponent.generated.h"

class FViewport;
class FCanvas;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGTDataReadySignature,
                                             const TArray<uint8> &, Data,
                                             const FDateTime &, TimeStamp);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALGT_API UGTDataGeneratorComponent : public USceneComponent {
  GENERATED_BODY()

public:
  FGTDataReadySignature DataReadyDelegate;

  // Sets default values for this component's properties
  UGTDataGeneratorComponent();

  virtual void DrawDebug(FViewport *Viewport, FCanvas *Canvas);

  virtual void GenerateData(const FDateTime &TimeStamp);

protected:
  // Called when the game starts
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void
  TickComponent(float DeltaTime, ELevelTick TickType,
                FActorComponentTickFunction *ThisTickFunction) override;
};
