// Fill out your copyright notice in the Description page of Project Settings.

#if WITH_EDITOR
#include "GTDebugWidget.h"

#include "Engine.h"
#include "Slate/SceneViewport.h"
#include "Widgets/Input/STextComboBox.h"
#include <Editor.h>
#include <UnrealClient.h>

#include "GTDebugViewportClient.h"
#include "Generators/GTDataGeneratorComponent.h"

void SGTDebugWidget::Construct(const FArguments& InArgs)
{
    // Add Placeholder until first generation
    GeneratorComponentPaths.Add(MakeShared<FString>(TEXT("Select Generator Component")));

    // clang-format off
    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
         SAssignNew(GeneratorComboBox, STextComboBox)
		    .OptionsSource(&GeneratorComponentPaths)
		    .InitiallySelectedItem(GeneratorComponentPaths[0])
		    .OnComboBoxOpening_Lambda([this](){UpdateGeneratorPathList();})
		    .OnSelectionChanged(this, &SGTDebugWidget::OnDataGeneratorPathUpdated)
        ]
        + SVerticalBox::Slot()
        .FillHeight(1.f)
        [
            SAssignNew(ViewportWidget, SViewport)
                .EnableGammaCorrection(false)
        ]
    ];
    // clang-format on
    ViewportClient = MakeShareable(new FGTDebugViewportClient());

    Viewport = MakeShareable(new FSceneViewport(ViewportClient.Get(), ViewportWidget));

    // The viewport widget needs an interface so it knows what should render
    ViewportWidget->SetViewportInterface(Viewport.ToSharedRef());
}

TSharedPtr<FSceneViewport> SGTDebugWidget::GetViewport() const
{
    return Viewport;
}

TSharedPtr<SViewport> SGTDebugWidget::GetViewportWidget() const
{
    return ViewportWidget;
}

void SGTDebugWidget::SetDataGenerator(UGTDataGeneratorComponent* DataGenerator)
{
    ((FGTDebugViewportClient*)Viewport->GetClient())->SetDataGenerator(DataGenerator);
}

void SGTDebugWidget::OnDataGeneratorPathUpdated(TSharedPtr<FString> NewPath, ESelectInfo::Type SelectType)
{
    UWorld* World = GEditor->PlayWorld;
    if (World)
    {
        for (TObjectIterator<UGTDataGeneratorComponent> Itr; Itr; ++Itr)
        {
            if (NewPath.IsValid() && Itr->GetWorld() == World && Itr->GetPathName() == *NewPath)
            {
                SetDataGenerator(*Itr);
            }
        }
    }
}

void SGTDebugWidget::Tick(
    const FGeometry& AllottedGeometry,
    const double InCurrentTime,
    const float InDeltaTime)
{
    Viewport->Invalidate();
    Viewport->InvalidateDisplay();
}

void SGTDebugWidget::UpdateGeneratorPathList()
{
    GeneratorComponentPaths.Reset();

    UWorld* World = GEditor->PlayWorld;
    if (World)
    {
        for (TObjectIterator<UGTDataGeneratorComponent> Itr; Itr; ++Itr)
        {
            if (Itr->GetWorld() == World)
            {
                GeneratorComponentPaths.Add(MakeShared<FString>(Itr->GetPathName()));
            }
        }
    }

    GeneratorComboBox->ClearSelection();
    GeneratorComboBox->RefreshOptions();
}

#endif