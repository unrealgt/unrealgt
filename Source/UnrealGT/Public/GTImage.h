// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
struct UNREALGT_API FGTImage {
public:
  int Width;
  int Height;
  TArray<FColor> Pixels;

  FGTImage();

  FGTImage(int Width, int Height);

  FGTImage(TArray<FColor> Pixels, int Width, int Height);

  FColor GetPixel(int X, int Y) const;

  void SetPixel(const FColor &PixelColor, int X, int Y);

  void SetSize(int NewWidth, int NewHeight);

  bool IsValid() const;

  /* Encodes the R channel of a FFLoat16Color in the R and G channel of a FColor
   */
  static void FromFloat16RChannel(TArray<FFloat16Color> InPixels, int InWidth,
                                  int InHeight, FGTImage &OutImage);
};
