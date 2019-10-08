// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"
#include "CoreMinimal.h"

#include "GTGeneratorReference.h"

#include "GTDataStreamerComponent.generated.h"

class UGTDataGeneratorComponent;

UCLASS(ClassGroup = (Custom), EditInlineNew,
       meta = (BlueprintSpawnableComponent))
class UNREALGT_API UGTDataStreamerComponent : public UActorComponent {
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UGTDataStreamerComponent();

  virtual void
  TickComponent(float DeltaTime, ELevelTick TickType,
                FActorComponentTickFunction *ThisTickFunction) override;

protected:
  virtual void BeginPlay() override;

  UFUNCTION()
  virtual void OnDataReady(const TArray<uint8> &Data,
                           const FDateTime &TimeStamp);

private:
  UPROPERTY(EditDefaultsOnly)
  FGTGeneratorReference GeneratorReference;
};
