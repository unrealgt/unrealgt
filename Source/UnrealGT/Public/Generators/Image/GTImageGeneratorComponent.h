// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Camera/CameraTypes.h>
#include <Engine/Scene.h>

#include "GTImageGeneratorBase.h"

#include "GTImageGeneratorComponent.generated.h"

class UGTSceneCaptureComponent2D;

/**
 * A virtual camera that enerates RGB images.
 * A lot of camera specific paramters like exposure, ISO, shutterspeed, bloom, fov (focal-length) can be configured.
 * These parameters can be found in the [Image](#Image), [PostProcessVolume](#PostProcessVolume) and [Projection](#Projection) Categories.<br>
 * For maximum compatibility the generated images are encoded in a standardized format currently BMP and PNG are supported.<br>
 * It is also possible to randomize resolution and gamma, which can bes useful for traning data, more ranomizable properties will be added in the future.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALGT_API UGTImageGeneratorComponent : public UGTImageGeneratorBase
{
    GENERATED_BODY()

public:
    /**
     * Use the display gamma for generated images usually around 2.4
     */
    UPROPERTY(EditAnywhere, Category = Image)
    bool bUseDisplayGamma;

    /**
     * Specify a custom gama value for images.
     */
    UPROPERTY(EditAnywhere, Category = Image, Meta = (EditCondition = "!bUseDisplayGamma"))
    float TargetGamma;

    /**
     * If enabled will choose a random gamma between TargetGamma and TargetGammaMax for each captured image.
     * This is useful for generating Training data.
     */
    UPROPERTY(EditAnywhere, Category = Image, Meta = (EditCondition = "!bUseDisplayGamma"))
    bool bUseRandomGamma;

    /**
     * The maximum gamma to use if bRandomResolution is enabled.
     */
    UPROPERTY(EditAnywhere, Category = Image, Meta = (EditCondition = "bUseRandomGamma"))
    float TargetGammaMax;

    /** Camera projection type. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projection, meta = (DisplayName = "Projection Type"))
    TEnumAsByte<ECameraProjectionMode::Type> ProjectionType;

    /** The desired width (in world units) of the orthographic view (ignored in Perspective mode) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projection)
    float OrthoWidth;

    /** Post-process settings to use [FPostProcessSettings](https://docs.unrealengine.com/en-US/API/Runtime/Engine/Engine/FPostProcessSettings/index.html)*/
    UPROPERTY(interp, Category = PostProcessVolume, meta = (ShowOnlyInnerProperties))
    FPostProcessSettings PostProcessSettings;

    /**
     * Enables a clip plane while rendering the scene capture which is useful for portals.
     * The global clip plane must be enabled in the renderer project settings for this to work.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = SceneCapture)
    bool bEnableClipPlane;

    /** Base position for the clip plane, can be any position on the plane. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = SceneCapture)
    FVector ClipPlaneBase;

    /** Normal for the plane. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = SceneCapture)
    FVector ClipPlaneNormal;

    UGTImageGeneratorComponent();

	virtual void GenerateData(const FDateTime& TimeStamp) override;

protected:
    virtual void BeginPlay() override;
};
