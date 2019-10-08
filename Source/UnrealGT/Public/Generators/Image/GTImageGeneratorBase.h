// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GTDataGeneratorComponent.h"
#include "GTImageFileFormat.h"

#include "GTImageGeneratorBase.generated.h"

class UTextureRenderTarget2D;
class UGTSceneCaptureComponent2D;

UCLASS(ClassGroup = (Custom), Abstract, hidecategories = (Collision, Object, Physics, SceneComponent, Rendering))
class UNREALGT_API UGTImageGeneratorBase : public UGTDataGeneratorComponent
{
    GENERATED_BODY()

public:
    UGTImageGeneratorBase();

    virtual void DrawDebug(FViewport* Viewport, FCanvas* Canvas) override;

    virtual void GenerateData(const FDateTime& TimeStamp) override;

    virtual UGTSceneCaptureComponent2D* GetSceneCaptureComponent() const;

    bool GetCalibrationParameters(FVector2D& OutFocalLength, FVector2D& OutPrincipalPoint) const;

    bool GetStereoCalibrationParameters(const UGTImageGeneratorBase* SecondCamera,
                                        FVector2D& OutFocalLengthOne,
                                        FVector2D& OutPrincipalPointOne,
                                        FVector2D& OutFocalLengthTwo,
                                        FVector2D& OutPrincipalPointTwo,
                                        FIntPoint& OutImageSize,
                                        FRotator& OutRotation,
                                        FVector& OutTranslation) const;

protected:
    UPROPERTY()
    UGTSceneCaptureComponent2D* SceneCaptureComponent;

    /**
     * The format the image will have, currently only .bmp and .png are supported.
     */
	UPROPERTY(EditDefaultsOnly, Category = Image)
    EGTImageFileFormat ImageFormat;

    /**
     * The resolution the generated images will have.
     */
    UPROPERTY(EditDefaultsOnly, Category = Image)
    FIntPoint Resolution;

    /**
     * If enabled will choose a random resolution between Resolution and ResolutionMax for each captured image.
     * This is useful for generating Training data.
     */
    UPROPERTY(EditDefaultsOnly, Category = Image)
    bool bRandomResolution;

    /**
     * The maximum Resolution to use if bRandomResolution is enabled.
     */
    UPROPERTY(EditDefaultsOnly, Category = Image, Meta = (EditCondition = "bRandomResolution"))
    FIntPoint ResolutionMax;

    /** Camera field of view (in degrees). */
    UPROPERTY(interp,
              EditAnywhere,
              Category = Projection,
              meta = (DisplayName = "Field of View", UIMin = "5.0", UIMax = "170", ClampMin = "0.001", ClampMax = "360.0"))
    float FOVAngle;

    /**
     * Perform anti aliasing when an image is rendered.
     * Highly recommended for basic images, but might cause issues if used with segmentation or depth generators.
     */
    UPROPERTY(EditDefaultsOnly, Category = Image)
    bool bAntiAliasing;

	UPROPERTY()
    bool bWriteAlpha;

    virtual void BeginPlay() override;

private:
    UPROPERTY()
    UStaticMeshComponent* CameraStaticMeshComponent;

    FIntPoint GenerateRandomResolution();
};
