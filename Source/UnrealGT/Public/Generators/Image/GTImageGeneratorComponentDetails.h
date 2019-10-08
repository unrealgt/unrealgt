// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#if WITH_EDITOR

#include "CoreMinimal.h"
#include <IDetailCustomization.h>
#include <Input/Reply.h>
#include "GTGeneratorReference.h"

class UGTImageGeneratorBase;

/**
 * 
 */
class UNREALGT_API FGTImageGeneratorComponentDetails : public IDetailCustomization
{
public:
    static TSharedRef<IDetailCustomization> MakeInstance();

    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

    FReply GenerateMonoCameraParameters();

    FReply GenerateStereoCameraParameters();

    UGTImageGeneratorBase* GetImageGeneratorFromReference(const FGTGeneratorReference& GeneratorReference) const;

private:
	IDetailLayoutBuilder* DetailBuilderCached;
};

#endif