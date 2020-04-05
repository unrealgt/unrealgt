// Fill out your copyright notice in the Description page of Project Settings.

#include "GTDebugViewportClient.h"

#include <CanvasItem.h>
#include <CanvasTypes.h>
#include <Engine/Engine.h>

#include "Generators/GTDataGeneratorComponent.h"

void FGTDebugViewportClient::Draw(FViewport* Viewport, FCanvas* Canvas)
{
    if (CurrentGenerator.IsValid())
    {
        CurrentGenerator->DrawDebug(Viewport, Canvas);
        Canvas->Clear(FLinearColor::White);
        FCanvasTextItem TextItem(
            FVector2D(Canvas->GetViewRect().Width(), 0.f),
            FText::FromString(CurrentGenerator->GetName()),
            GEngine->GetMediumFont(),
            FLinearColor::Red);
        Canvas->DrawItem(TextItem);
    }
}

void FGTDebugViewportClient::SetDataGenerator(UGTDataGeneratorComponent* DataGenerator)
{
    CurrentGenerator = DataGenerator;
}
