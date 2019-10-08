// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GTDataGeneratorComponent.h"
#include "GTGeneratorReference.h"
#include "GTObjectFilter.h"
#include "GTImage.h"

#include "GTActorInfoGeneratorComponent.generated.h"

class UGTSceneCaptureComponent2D;
class UGTImageGeneratorBase;

/**
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), hidecategories = (Collision, Object, Physics, SceneComponent))
class UNREALGT_API UGTActorInfoGeneratorComponent : public UGTDataGeneratorComponent
{
    GENERATED_BODY()
public:
    UGTActorInfoGeneratorComponent();

    UPROPERTY(EditAnywhere, Category = "Tracked Actors")
    TArray<FGTObjectFilter> TrackActorsThatMatchFilter;

    UPROPERTY(EditAnywhere, Category = "Tracked Actors")
    bool bOnlyTrackRecentlyRenderedActors;

    UPROPERTY(EditAnywhere, Category = "Tracked Actors")
    bool bOnlyTrackOnScreenActors;

    UPROPERTY(EditAnywhere, Category = "Tracked Actors")
    FVector2D MinimalRequiredBoundingBoxSize;

    UPROPERTY(EditAnywhere, Category = "Tracked Actors")
    float MaxDistanceToCamera;

    /**
     * This will drastically increase bounding box accuracy but requires an additional render pass. The Larger the resolution
     * of the LinkedGenerator the larger the performance hit.
     */
    UPROPERTY(EditAnywhere, Category = "Output Format")
    bool bAccurateBoundingBoxes;

    /** Apply Close for Bounding Box Segmentation */
    UPROPERTY(EditAnyWhere, Category = "Output Format", Meta = (DisplayName = "Apply Closing (Dilation & Erosion) for Bounding Box calculation", EditCondtion = "bAccurateBoundingBoxes"))
    bool bShouldApplyCloseMorph;

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

protected:
    void BeginPlay() override;

private:
    FString CurrentResult;

    UPROPERTY()
    UGTSceneCaptureComponent2D* SegmentationSceneCapture;

    FGTImage CachedSegmentation;

    TMap<AActor*, FBox2D> CachedBoundingBoxes;

    bool IsActorRenderedOnScreen(AActor* Actor, float DeltaTime);

    bool GetActorScreenBoundingBox(AActor* InActor, UGTImageGeneratorBase* ImageGeneratorComponent, FBox2D& OutBox);

    FString Vector2DToFormattedString(const FVector2D& InVector);

    FString Vector3DToFormattedString(const FVector& InVector);

    FString RotatorToFormattedString(const FRotator& InRotator);

    FString Box2DToFormattedString(const FBox2D& InBox);

    FString Box3DToFormattedString(const FBox& InBox);
};
