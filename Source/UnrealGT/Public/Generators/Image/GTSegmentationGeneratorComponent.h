// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GTImageGeneratorBase.h"
#include "GTObjectFilter.h"

#include "GTSegmentationGeneratorComponent.generated.h"

class UGTSceneCaptureComponent2D;

/**
 * Can generate segmentation for an image. You can assign components color by adding a filter color
 * pair to the ComponentToColor map.
 * **Requires "Enabled with stencil" project setting in `Engine > Rendering > Postprocessing >
 * Custom-Depth Stencil Pass`**
 */
UCLASS(
    ClassGroup = (Custom),
    meta = (BlueprintSpawnableComponent),
    hidecategories = (Collision, Object, Physics, SceneComponent))
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

    UPROPERTY(
        EditAnyWhere,
        Category = Segmentaiton,
        Meta = (DisplayName = "Apply Closing (Dilation & Erosion)"))
    bool bShouldApplyCloseMorph;
};
