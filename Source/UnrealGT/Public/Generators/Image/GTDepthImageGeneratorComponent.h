// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GTImageGeneratorBase.h"

#include "GTDepthImageGeneratorComponent.generated.h"

/**
 * Generates depth images. Uses planar depth as default setting, perspective depth can be enabled
 * via bUsePerspectiveDepth.<br> The depth is encoded in millimeters (mm) in the RGB channels of the
 * image. This means the depth value is a 24bit with a maximum value of 16.777216 km. (TODO add
 * nanometer option). The RGB values can be converted to mm using the following formula:
 * `R + G * 256 + B * 256 * 256`.
 * See https://github.com/unrealgt/unrealgt/blob/master/Examples/PythonDepthConvert/main.py for an
 * example.
 *
 * **Note:** Anti-Aliasing is disabled for depth generators by default!
 */
UCLASS(
    ClassGroup = (Custom),
    meta = (BlueprintSpawnableComponent),
    hideCategories =
        (Lens,
         ColorGrading,
         Film,
         MobileTonemapper,
         RenderingFeatures,
         PostProcessVolume,
         SceneCapture,
         PlanarReflection))
class UNREALGT_API UGTDepthImageGeneratorComponent : public UGTImageGeneratorBase
{
    GENERATED_BODY()

public:
    /** Use perspective depth instead of planar depth. */
    UPROPERTY(EditAnywhere, Category = "Depth")
    bool bUsePerspectiveDepth;

    /** Maximum depth(distance) to record. */
    UPROPERTY(EditAnywhere, Category = "Depth")
    float MaxZ;

    UGTDepthImageGeneratorComponent();

    virtual void DrawDebug(FViewport* Viewport, FCanvas* Canvas) override;

protected:
    virtual void BeginPlay() override;
};
