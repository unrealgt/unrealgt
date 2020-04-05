// Fill out your copyright notice in the Description page of Project Settings.

#if WITH_EDITOR
#include "GTDebugWidget.h"

#include "Engine.h"
#include "Slate/SceneViewport.h"
#include <Components/PrimitiveComponent.h>
#include <Editor.h>
#include <UnrealClient.h>

#include "GTDebugViewportClient.h"
#include "Generators/GTDataGeneratorComponent.h"

void SGTDebugWidget::Construct(const FArguments& InArgs)
{
    // clang-format off
    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SEditableTextBox)
            .OnTextChanged(this, &SGTDebugWidget::OnDataGeneratorNameUpdated)
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

void SGTDebugWidget::OnDataGeneratorNameUpdated(const FText& NewText)
{
    UWorld* World = GEditor->PlayWorld;
    if (World)
    {
        for (TObjectIterator<UGTDataGeneratorComponent> Itr; Itr; ++Itr)
        {
            if (Itr->GetWorld() == World && Itr->GetName().Contains(NewText.ToString()))
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

#endif