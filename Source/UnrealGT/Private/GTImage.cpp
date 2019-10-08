// Fill out your copyright notice in the Description page of Project Settings.

#include "GTImage.h"

FGTImage::FGTImage() : Width(0), Height(0) {}

FGTImage::FGTImage(int Width, int Height) { SetSize(Width, Height); }

FGTImage::FGTImage(TArray<FColor> Pixels, int Width, int Height)
    : Width(Width), Height(Height), Pixels(Pixels) {}

FColor FGTImage::GetPixel(int X, int Y) const { return Pixels[X + Y * Width]; }

void FGTImage::SetPixel(const FColor &PixelColor, int X, int Y) {
  Pixels[X + Y * Width] = PixelColor;
}

void FGTImage::SetSize(int NewWidth, int NewHeight) {
  Width = NewWidth;
  Height = NewHeight;
  Pixels.Reserve(Width * Height);
}

bool FGTImage::IsValid() const {
  if (Pixels.Num() == 0 || Width <= 0 || Height <= 0) {
    return false;
  }
  return true;
}

void FGTImage::FromFloat16RChannel(TArray<FFloat16Color> InPixels, int InWidth,
                                   int InHeight, FGTImage &OutImage) {
  OutImage.Pixels.Reserve(InWidth * InHeight);
  for (const FFloat16Color &InPixel : InPixels) {
    OutImage.Pixels.Push(
        FColor(InPixel.R.Encoded & 0xff, InPixel.R.Encoded >> 8, 0, 255));
  }
}
