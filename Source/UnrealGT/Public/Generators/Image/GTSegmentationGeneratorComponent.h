// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GTImageGeneratorBase.h"
#include "GTObjectFilter.h"

#include "GTSegmentationGeneratorComponent.generated.h"

class UGTSceneCaptureComponent2D;

/**
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), hidecategories = (Collision, Object, Physics, SceneComponent))
class UNREALGT_API UGTSegmentationGeneratorComponent : public UGTImageGeneratorBase
{
    GENERATED_BODY()
public:
    UGTSegmentationGeneratorComponent();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditAnyWhere, Category = Segmentaiton)
    TMap<FGTObjectFilter, FColor> ComponentToColor;

    UPROPERTY(EditAnyWhere, Category = Segmentaiton, Meta = (DisplayName = "Apply Closing (Dilation & Erosion)"))
    bool bShouldApplyCloseMorph;
};
