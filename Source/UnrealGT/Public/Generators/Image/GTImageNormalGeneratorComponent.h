// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GTImageGeneratorBase.h"

#include "GTImageNormalGeneratorComponent.generated.h"

/**
 * Renders the surface normals of the objects in the scene. The normals are encoded in the RGB channels.
 * They can be converted from RGB to world space coordinates using: `RGB -> Normal: 2 * C/255 C - 1`
 * <br>
 * **Note:** Anti-Aliasing is disabled for normal generators by default!
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALGT_API UGTImageNormalGeneratorComponent : public UGTImageGeneratorBase
{
    GENERATED_BODY()
public:
    UGTImageNormalGeneratorComponent();
};
