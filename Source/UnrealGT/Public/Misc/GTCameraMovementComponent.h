// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "GTCameraMovementComponent.generated.h"

class ACameraRig_Rail;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALGT_API UGTCameraMovementComponent : public UActorComponent {
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UGTCameraMovementComponent();

  // Called every frame
  virtual void
  TickComponent(float DeltaTime, ELevelTick TickType,
                FActorComponentTickFunction *ThisTickFunction) override;

protected:
  virtual void BeginPlay() override;

private:
  UPROPERTY(EditAnywhere, Category = CaptureSettings,
            meta = (EditCondition = "!bFollowActor && !bFollowRail"))
  bool bFollowMainView;

  UPROPERTY(EditAnywhere, Category = CaptureSettings,
            meta = (EditCondition = "!bFollowMainView && !bFollowRail"))
  bool bFollowActor;

  UPROPERTY(EditAnywhere, Category = CaptureSettings,
            meta = (EditCondition = "!bFollowMainView && !bFollowRail"))
  AActor *FollowActor;

  UPROPERTY(EditAnywhere, Category = CaptureSettings,
            meta = (EditCondition = "!bFollowMainView && !bFollowActor"))
  bool bFollowRail;

  UPROPERTY(EditAnywhere, Category = CaptureSettings,
            meta = (EditCondition = "!bFollowMainView && !bFollowActor"))
  bool bLockOrientationToRail;

  // TODO readd AllowedClasses = "ACameraRig_Rail"
  UPROPERTY(EditAnywhere, Category = CaptureSettings,
            meta = (EditCondition = "!bFollowMainView && !bFollowActor"))
  ACameraRig_Rail *FollowRail;

  UPROPERTY(EditAnywhere, Category = CaptureSettings,
            meta = (EditCondition = "!bFollowMainView && !bFollowActor"))
  float RailSpeed;

  UPROPERTY(EditAnywhere, Category = CaptureSettings,
            meta = (EditCondition = "!bFollowMainView && !bFollowActor"))
  bool bDestroyOwnerAtEnd;

  float AverageDirectionChange;
};
