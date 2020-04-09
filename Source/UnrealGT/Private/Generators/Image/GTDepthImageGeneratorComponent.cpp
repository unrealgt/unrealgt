// Fill out your copyright notice in the Description page of Project Settings.

#include "Generators/Image/GTDepthImageGeneratorComponent.h"

#include <CanvasItem.h>
#include <CanvasTypes.h>
#include <Engine/TextureRenderTarget2D.h>

#include "GTImage.h"
#include "Generators/Image/GTSceneCaptureComponent2D.h"

UGTDepthImageGeneratorComponent::UGTDepthImageGeneratorComponent()
    : Super()
    , MaxZ(100000.f)
{
    bAntiAliasing = false;
}

void UGTDepthImageGeneratorComponent::DrawDebug(FViewport* Viewport, FCanvas* Canvas)
{
    if (SceneCaptureComponent && SceneCaptureComponent->TextureTarget &&
        SceneCaptureComponent->TextureTarget->IsValidLowLevel())
    {
        UTextureRenderTarget2D* DebugTextureTarget = SceneCaptureComponent->TextureTarget;
        FTexture* RenderTextureResource = DebugTextureTarget->Resource;
        FCanvasTileItem TileItem(
            FVector2D::ZeroVector,
            RenderTextureResource,
            FVector2D(DebugTextureTarget->SizeX, DebugTextureTarget->SizeY),
            FVector2D::ZeroVector,
            FVector2D::ZeroVector + FVector2D(1.f, 1.f),
            FLinearColor::White);
        TileItem.Rotation = FRotator(0.f, 0.f, 0.f);
        TileItem.PivotPoint = FVector2D(0.5f, 0.5f);
        TileItem.BlendMode = FCanvas::BlendToSimpleElementBlend(EBlendMode::BLEND_Opaque);
        Canvas->DrawItem(TileItem);

        int Width = DebugTextureTarget->SizeX;

        int Height = DebugTextureTarget->SizeY;

        FGTImage DepthImage(Width, Height);
        FTextureRenderTargetResource* RenderTargetResource =
            DebugTextureTarget->GameThread_GetRenderTargetResource();

        FReadSurfaceDataFlags ReadSurfaceDataFlags(RCM_UNorm);
        ReadSurfaceDataFlags.SetLinearToGamma(false);

        RenderTargetResource->ReadPixels(DepthImage.Pixels, ReadSurfaceDataFlags);

        for (int PixelID = 0; PixelID < DepthImage.Pixels.Num(); PixelID++)
        {
            FColor PixelColor = DepthImage.Pixels[PixelID];
            float PixelDepth =
                ((PixelColor.R + PixelColor.G * 256.f + PixelColor.B * 256.f * 256.f) /
                 (MaxZ * 10.f)) *
                1000.f;
            float PixelDepthLogScale =
                FMath::Clamp(FMath::Loge(PixelDepth) / 9.72777705446f, 0.f, 1.f) * 255;
            DepthImage.Pixels[PixelID] =
                FColor(PixelDepthLogScale, PixelDepthLogScale, PixelDepthLogScale, 255);
        }

        UTexture2D* ConvertedDepthTexture =
            GetSceneCaptureComponent()->TextureFromImage(DepthImage);
        FTexture* ConvertedDepthTextureTextureResource = ConvertedDepthTexture->Resource;
        FCanvasTileItem ConvertedDepthTileItem(
            FVector2D(TileItem.Size.X, 0.f),
            ConvertedDepthTextureTextureResource,
            FVector2D(DepthImage.Width, DepthImage.Height),
            FVector2D::ZeroVector,
            FVector2D::ZeroVector + FVector2D(1.f, 1.f),
            FLinearColor::White);
        ConvertedDepthTileItem.Rotation = FRotator(0.f, 0.f, 0.f);
        ConvertedDepthTileItem.PivotPoint = FVector2D(0.5f, 0.5f);
        ConvertedDepthTileItem.BlendMode =
            FCanvas::BlendToSimpleElementBlend(EBlendMode::BLEND_Opaque);
        Canvas->DrawItem(ConvertedDepthTileItem);
    }
}

void UGTDepthImageGeneratorComponent::BeginPlay()
{
    Super::BeginPlay();

    SceneCaptureComponent->SetupDepthPostProccess(MaxZ, bUsePerspectiveDepth);
}
