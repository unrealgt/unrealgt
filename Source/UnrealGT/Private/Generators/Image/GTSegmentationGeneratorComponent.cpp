// Fill out your copyright notice in the Description page of Project Settings.

#include "Generators/Image/GTSegmentationGeneratorComponent.h"

#include "Algo/Accumulate.h"
#include "CanvasItem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Serialization/JsonSerializerMacros.h"
#include "Serialization/JsonWriter.h"

#include "GTFileUtilities.h"
#include "Generators/Image/GTSceneCaptureComponent2D.h"

UGTSegmentationGeneratorComponent::UGTSegmentationGeneratorComponent()
    : Super()
{
    bAntiAliasing = false;
}

FString UGTSegmentationGeneratorComponent::GenerateSegmentationInfoJSON() const
{
    FString SegmentationJSONInformation;

    TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> JsonWriter =
        TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(
            &SegmentationJSONInformation);
    FJsonSerializerWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>> Serializer(JsonWriter);
    Serializer.StartArray();
    for (auto ComponentColorPair : GetSceneCaptureComponent()->ComponentToColor)
    {
        const auto Component = ComponentColorPair.Key;
        Serializer.StartObject();

        auto Name = Component->GetName();
        Serializer.Serialize(TEXT("Name"), Name);

        auto Path = Component->GetPathName();
        Serializer.Serialize(TEXT("Path"), Path);

        const auto StaticMeshComponent = Cast<UStaticMeshComponent>(Component);
        if (StaticMeshComponent)
        {
            auto MeshName = StaticMeshComponent->GetStaticMesh()->GetName();
            Serializer.Serialize(TEXT("Mesh"), MeshName);
        }

        const auto SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Component);
        if (SkeletalMeshComponent)
        {
            auto MeshName = SkeletalMeshComponent->SkeletalMesh->GetName();
            Serializer.Serialize(TEXT("SkeletalMesh"), MeshName);
        }

        TArray<FString> Tags;
        Tags.Reserve(Component->ComponentTags.Num());
        for (const auto Tag : Component->ComponentTags)
        {
            Tags.Push(Tag.ToString());
        }
        Serializer.SerializeArray(TEXT("Tags"), Tags);

        auto Color = ComponentColorPair.Value.ToString();
        Serializer.Serialize(TEXT("Color"), Color);
        Serializer.EndObject();
    }
    Serializer.EndArray();
    JsonWriter->Close();

    return SegmentationJSONInformation;
}

void UGTSegmentationGeneratorComponent::BeginPlay()
{
    Super::BeginPlay();

    SceneCaptureComponent->SetupSegmentationPostProccess(
        ComponentToColor,
        bShouldApplyCloseMorph,
        bColorEachComponentDifferent,
        bUseFilterForColorEachComponentDifferent,
        ColorEachComponentDifferentFilter);

    DebugComponentToColorString = Algo::TransformAccumulate(
        GetSceneCaptureComponent()->ComponentToColor,
        [](const auto ComponentColorPair) {
            return ComponentColorPair.Key->GetPathName() + TEXT(": ") +
                   ComponentColorPair.Value.ToString() + TEXT("\n");
        },
        FString(TEXT("")));

    FGTFileUtilities::WriteFileToSessionDirectory(
        FPaths::Combine(GetName(), TEXT("segmentation_info.json")),
        FGTFileUtilities::StringToCharArray(GenerateSegmentationInfoJSON()),
        GetWorld());
}

void UGTSegmentationGeneratorComponent::DrawDebug(FViewport* Viewport, FCanvas* Canvas)
{
    Super::DrawDebug(Viewport, Canvas);

    if (SceneCaptureComponent && SceneCaptureComponent->TextureTarget &&
        SceneCaptureComponent->TextureTarget->IsValidLowLevel())
    {
        UTextureRenderTarget2D* DebugTextureTarget = SceneCaptureComponent->TextureTarget;

        FCanvasTextItem TextItem(
            FVector2D(DebugTextureTarget->GetResource()->GetSizeX(), 0.f),
            FText::FromString(DebugComponentToColorString.Mid(0, 5000)),
            GEngine->GetMediumFont(),
            FLinearColor::Red);
        Canvas->DrawItem(TextItem);
    }
}
