// Fill out your copyright notice in the Description page of Project Settings.

#include "Generators/Image/GTSegmentationGeneratorComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonSerializerMacros.h"
#include "Serialization/JsonWriter.h"

#include "GTFileUtilities.h"
#include "Generators/Image/GTSceneCaptureComponent2D.h"

UGTSegmentationGeneratorComponent::UGTSegmentationGeneratorComponent()
    : Super()
{
    bAntiAliasing = false;
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

    FString SegmentationJSONInformation;
    TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
        TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(
            &SegmentationJSONInformation);
    FJsonSerializerWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> Serializer(JsonWriter);
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

    FGTFileUtilities::WriteFileToSessionDirectory(
        FPaths::Combine(GetName(), TEXT("segmentation_info.json")),
        FGTFileUtilities::StringToCharArray(SegmentationJSONInformation),
        GetWorld());
}
