// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GTGeneratorReference.h"
#include "GTImage.h"
#include "GTObjectFilter.h"
#include "Generators/GTDataGeneratorComponent.h"

#include "GTActorInfoGeneratorComponent.generated.h"

class UGTSceneCaptureComponent2D;
class UGTImageGeneratorBase;

/**
 * Generates information about actors in a scene (e.g. MeshName, BoundingBoxes, ActorName).
 * The Output format can be customize via formatting strings.
 */
UCLASS(
    ClassGroup = (Custom),
    meta = (BlueprintSpawnableComponent),
    hidecategories = (Collision, Object, Physics, SceneComponent))
class UNREALGT_API UGTActorInfoGeneratorComponent : public UGTDataGeneratorComponent
{
    GENERATED_BODY()
public:
    UGTActorInfoGeneratorComponent();

    UPROPERTY(EditAnywhere, Category = "Tracked Actors")
    TArray<FGTObjectFilter> TrackActorsThatMatchFilter;
    
    /**
     * Requires LinkedImageGenerator to be set.
     */
    UPROPERTY(EditAnywhere, Category = "Tracked Actors")
    bool bOnlyTrackOnScreenActors = false;

    UPROPERTY(EditAnywhere, Category = "Tracked Actors", meta=(EditCondition="bOnlyTrackOnScreenActors"))
    bool bRequireMinimumVisibleBoundingBox = false;
    
    UPROPERTY(EditAnywhere, Category = "Tracked Actors", meta=(EditCondition="bRequireMinimumVisibleBoundingBox"))
    FVector2D MinimalRequiredBoundingBoxSize;

    UPROPERTY(EditAnywhere, Category = "Tracked Actors")
    float MaxDistanceToCamera;

    /**
     * Drastically Increase the accuracy of the bounding boxes, but requires an additional render
     * pass. The additional render pass creates a segmentation map for the tracked actors and uses
     * that map to refine the bounding boxes.
     *
     * **Therefore Requires "Enabled with stencil" in `Engine > Rendering > Postprocessing >
     * Custom-Depth Stencil Pass` to be set**
     *
     * The additional render pass doubles the cost of creating the bounding boxes.
     * The Larger the resolution of the LinkedGenerator the larger the performance hit.
     */
    UPROPERTY(EditAnywhere, Category = "Output Format")
    bool bAccurateBoundingBoxes;

    /** Apply Close for Bounding Box Segmentation */
    UPROPERTY(
        EditAnyWhere,
        Category = "Output Format",
        Meta =
            (DisplayName = "Apply Closing (Dilation & Erosion) for Bounding Box calculation",
             EditCondtion = "bAccurateBoundingBoxes"))
    bool bShouldApplyCloseMorph;

    /**
     * Link an image generator to this component.
     * The linked Generator is used to determine when an
     * actor is "on screen" and should be tracked.
     * Setting this is also required for generating 2D
     * bounding boxes.
     */
    UPROPERTY(EditAnywhere, Category = "Output Format")
    FGTGeneratorReference LinkedImageGenerator;

    UPROPERTY(EditAnywhere, Category = "Output Format", meta = (MultiLine = "true"))
    FString Header;

    UPROPERTY(EditAnywhere, Category = "Output Format", meta = (MultiLine = "true"))
    FString FormatActorString;

    UPROPERTY(EditAnywhere, Category = "Output Format", meta = (MultiLine = "true"))
    FString Separator;

    UPROPERTY(EditAnywhere, Category = "Output Format", meta = (MultiLine = "true"))
    FString Footer;

    UPROPERTY(EditAnywhere, Category = "Output Format", meta = (MultiLine = "true"))
    FString FormatVector2DString;

    UPROPERTY(EditAnywhere, Category = "Output Format", meta = (MultiLine = "true"))
    FString FormatVector3DString;

    UPROPERTY(EditAnywhere, Category = "Output Format", meta = (MultiLine = "true"))
    FString FormatRotatorString;

    UPROPERTY(EditAnywhere, Category = "Output Format", meta = (MultiLine = "true"))
    FString Format2DBoxString;

    UPROPERTY(EditAnywhere, Category = "Output Format", meta = (MultiLine = "true"))
    FString Format3DBoxString;

    UPROPERTY(EditAnywhere, Category = "Output Format")
    TMap<FString, FString> ReplaceStrings;

    virtual void GenerateData(const FDateTime& TimeStamp);

    virtual void DrawDebug(FViewport* Viewport, FCanvas* Canvas) override;

#ifdef WITH_EDITOR
    virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

protected:
    void BeginPlay() override;

private:
    FString CurrentResult;

    UPROPERTY()
    UGTSceneCaptureComponent2D* SegmentationSceneCapture;

    FGTImage CachedSegmentation;

    TMap<AActor*, FBox2D> CachedBoundingBoxes;

    bool IsActorRenderedOnScreen(AActor* Actor);

    bool GetActorScreenBoundingBox(
        AActor* InActor,
        UGTImageGeneratorBase* ImageGeneratorComponent,
        FBox2D& OutBox);

    FString Vector2DToFormattedString(const FVector2D& InVector);

    FString Vector3DToFormattedString(const FVector& InVector);

    FString RotatorToFormattedString(const FRotator& InRotator);

    FString Box2DToFormattedString(const FBox2D& InBox);

    FString Box3DToFormattedString(const FBox& InBox);
};
