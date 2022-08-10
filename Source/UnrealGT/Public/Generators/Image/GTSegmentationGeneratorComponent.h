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
 * <br><br>
 * Will emit a JSON file containing the configured color mappings
 * called "segmentation_info" in `Saved/UnrealGT/{SessionName}/{Time}/{SegmentationGeneratorName}`
 * <br><br>
 * **Requires "Enabled with stencil" project setting in `Engine > Rendering > Postprocessing >
 * Custom-Depth Stencil Pass` to be set**
 * <br><br>
 * **Note:** Anti-Aliasing is disabled for segmentation generators by default!
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

    UFUNCTION(BlueprintCallable, Category = UnrealGTSegmentation)
    void RegisterForSegmentation(UPrimitiveComponent* PrimitiveComponent);

    virtual void GenerateData(const FDateTime& TimeStamp) override;

protected:
    FString GenerateSegmentationInfoJSON() const;

    virtual void BeginPlay() override;

    virtual void DrawDebug(FViewport* Viewport, FCanvas* Canvas) override;

private:
    /**
     * Assigns a segmentation color to the component/mesh if it matches the corresponding filter.
     */
    UPROPERTY(
        EditAnyWhere,
        Category = Segmentation,
        Meta =
            (DisplayName = "Component to color Map (limted to 255 colors)",
             EditCondition = "!bColorEachComponentDifferent"))
    TMap<FGTObjectFilter, FColor> ComponentToColor;

    /**
     * This can improve segmentation quality for partially translucent meshes (e.g. Fences,
     * Foliage...)
     */
    UPROPERTY(
        EditAnyWhere,
        Category = Segmentation,
        Meta = (DisplayName = "Apply Closing (Dilation & Erosion)"))
    bool bShouldApplyCloseMorph;

    UPROPERTY(
        EditAnyWhere,
        Category = Segmentation,
        Meta = (DisplayName = "Use random color for every mesh (limited to 255 colors)"))
    bool bColorEachComponentDifferent;

    UPROPERTY(
        EditAnyWhere,
        Category = Segmentation,
        Meta =
            (DisplayName = "Apply random color only to components that match filter",
             EditCondition = "bColorEachComponentDifferent"))
    bool bUseFilterForColorEachComponentDifferent;

    UPROPERTY(
        EditAnyWhere,
        Category = Segmentation,
        Meta =
            (DisplayName = "Component filter for random colors",
             EditCondition =
                 "bColorEachComponentDifferent && bUseFilterForColorEachComponentDifferent"))
    FGTObjectFilter ColorEachComponentDifferentFilter;

    FString DebugComponentToColorString;
    TArray<UPrimitiveComponent*> WaitingToBeRegistered;
};
