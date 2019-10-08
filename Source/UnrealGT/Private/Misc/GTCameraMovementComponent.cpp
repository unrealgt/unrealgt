// Fill out your copyright notice in the Description page of Project Settings.

#include "GTCameraMovementComponent.h"

#include <CinematicCamera/Public/CameraRig_Rail.h>
#include <Components/SplineComponent.h>
#include <Engine/World.h>
#include <GameFramework/PlayerController.h>

UGTCameraMovementComponent::UGTCameraMovementComponent()
    : bFollowMainView(true), bLockOrientationToRail(true), RailSpeed(10.f),
      AverageDirectionChange(0.f) {
  // Set this component to be initialized when the game starts, and to be ticked
  // every frame.  You can turn these features off to improve performance if you
  // don't need them.
  PrimaryComponentTick.bCanEverTick = true;

  // ...
}

void UGTCameraMovementComponent::TickComponent(
    float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction *ThisTickFunction) {
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  if (bFollowMainView) {
    APlayerController *PlayerOne = GetWorld()->GetFirstPlayerController();

    if (PlayerOne) {
      FollowActor = PlayerOne->GetViewTarget();
      GetOwner()->SetActorLocationAndRotation(
          PlayerOne->PlayerCameraManager->GetCameraLocation(),
          PlayerOne->PlayerCameraManager->GetCameraRotation());
    }
  } else if (bFollowRail) {
    if (FollowRail) {
      float Progress =
          (RailSpeed /
           FollowRail->GetRailSplineComponent()->GetSplineLength()) *
          DeltaTime;

      float NewPosition =
          FMath::Min(FollowRail->CurrentPositionOnRail + Progress, 1.f);

      // we use our own lock rotation because the implementation inside
      // camerarigrail does not interpolate which resutls in stuttering
      if (bLockOrientationToRail) {
        FQuat const RailRot =
            FollowRail->GetRailSplineComponent()
                ->GetQuaternionAtDistanceAlongSpline(
                    FollowRail->GetRailSplineComponent()->GetSplineLength() *
                        NewPosition,
                    ESplineCoordinateSpace::World);
        GetOwner()->SetActorRotation(FMath::QInterpTo(
            GetOwner()->GetActorQuat(), RailRot, DeltaTime, 1.5f));
      }

      FollowRail->CurrentPositionOnRail = NewPosition;

      if (bDestroyOwnerAtEnd &&
          FMath::IsNearlyEqual(FollowRail->CurrentPositionOnRail, 1.f)) {
        GetOwner()->Destroy();
      }
    }
  }
}

void UGTCameraMovementComponent::BeginPlay() {
  Super::BeginPlay();

  if (bFollowRail) {
    if (FollowRail) {
      GetOwner()->AttachToComponent(
          FollowRail->GetDefaultAttachComponent(),
          FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    }
  }
}
