// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "GTGeneratorReference.h"

#include "GTGeneratorTrigger.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALGT_API UGTGeneratorTrigger : public UActorComponent {
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UGTGeneratorTrigger();

  UFUNCTION(BlueprintCallable, Category = Triggers)
  virtual void Trigger();

protected:
  // Called when the game starts
  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = TriggerSettings, meta = (EditCondition = "!bTriggerAllGeneratorComponents"))
  TArray<FGTGeneratorReference> DataGenerators;

};
