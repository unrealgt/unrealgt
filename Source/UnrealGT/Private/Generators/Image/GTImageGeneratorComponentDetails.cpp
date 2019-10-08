// Fill out your copyright notice in the Description page of Project Settings.
#if WITH_EDITOR

#include "GTImageGeneratorComponentDetails.h"

#include <AssertionMacros.h>
#include <DeclarativeSyntaxSupport.h>
#include <DetailCategoryBuilder.h>
#include <DetailLayoutBuilder.h>
#include <DetailWidgetRow.h>
#include <Engine/BlueprintGeneratedClass.h>
#include <Engine/SCS_Node.h>
#include <IDetailGroup.h>
#include <Runtime/ApplicationCore/Public/HAL/PlatformApplicationMisc.h>
#include <SBox.h>
#include <SButton.h>
#include <STextBlock.h>
#include <SUniformGridPanel.h>

#include "GTCameraCalibrationComponent.h"
#include "GTImageGeneratorBase.h"
#include "GTSceneCaptureComponent2D.h"

TSharedRef<IDetailCustomization>
FGTImageGeneratorComponentDetails::MakeInstance() {
  return MakeShareable(new FGTImageGeneratorComponentDetails());
}

void FGTImageGeneratorComponentDetails::CustomizeDetails(
    IDetailLayoutBuilder &DetailBuilder) {
  TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
  DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

  if (ObjectsBeingCustomized.Num() != 1) {
    return;
  }

  DetailBuilderCached = &DetailBuilder;

  IDetailCategoryBuilder &Cat = DetailBuilder.EditCategory(TEXT("Cameras"));

  Cat.AddProperty(GET_MEMBER_NAME_CHECKED(UGTCameraCalibrationComponent,
                                          CameraOneReference),
                  UGTCameraCalibrationComponent::StaticClass());
  Cat.AddProperty(GET_MEMBER_NAME_CHECKED(UGTCameraCalibrationComponent,
                                          CameraTwoReference),
                  UGTCameraCalibrationComponent::StaticClass());

  Cat.AddCustomRow(NSLOCTEXT("CameraCalibrationComponentDetails",
                             "CameraParameterButtons",
                             "Copy Camera Parameters to Clipboard"))
      .WholeRowContent()
      .HAlign(HAlign_Left)[SNew(SBox).MaxDesiredWidth(
          300.f)[SNew(SUniformGridPanel).SlotPadding(2.0f) +
                 SUniformGridPanel::Slot(0, 0)
                     [SNew(STextBlock)
                          .Text(NSLOCTEXT("CameraCalibrationComponentDetails",
                                          "CameraParameterButtonsHeader",
                                          "Copy to Clipboard"))] +
                 SUniformGridPanel::Slot(0, 1)
                     [SNew(SButton)
                          .OnClicked(this, &FGTImageGeneratorComponentDetails::
                                               GenerateMonoCameraParameters)
                          .ToolTipText(NSLOCTEXT(
                              "CameraCalibrationComponentDetails",
                              "CopyCameraOneButtonTooltip",
                              "Copy Parameters of CameraOne to the Clipboard"))
                          .HAlign(HAlign_Center)
                              [SNew(STextBlock)
                                   .Text(NSLOCTEXT(
                                       "CameraCalibrationComponentDetails",
                                       "CopyCameraOneButton",
                                       "CameraOne Parameters"))]] +
                 SUniformGridPanel::Slot(1, 1)
                     [SNew(SButton)
                          .OnClicked(this, &FGTImageGeneratorComponentDetails::
                                               GenerateStereoCameraParameters)
                          .ToolTipText(
                              NSLOCTEXT("CameraCalibrationComponentDetails",
                                        "CopyStereoButtonTooltip",
                                        "Copy Stereo Parameters of CameraOne "
                                        "and Two to the Clipboard"))
                          .HAlign(HAlign_Center)
                              [SNew(STextBlock)
                                   .Text(NSLOCTEXT(
                                       "CameraCalibrationComponentDetails",
                                       "CopyStereoButton",
                                       "Stereo Parameters"))]]]];
}

FReply FGTImageGeneratorComponentDetails::GenerateMonoCameraParameters() {
  TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
  DetailBuilderCached->GetObjectsBeingCustomized(ObjectsBeingCustomized);

  UGTCameraCalibrationComponent *CalibrationComponent =
      Cast<UGTCameraCalibrationComponent>(ObjectsBeingCustomized[0].Get());

  UGTImageGeneratorBase *Generator =
      GetImageGeneratorFromReference(CalibrationComponent->CameraOneReference);

  if (!Generator) {
    return FReply::Handled();
  };

  FVector2D FocalLength;
  FVector2D PrincipalPoint;
  if (!Generator->GetCalibrationParameters(FocalLength, PrincipalPoint)) {
    return FReply::Handled();
  };
  FString CameraParameterString = FString::Format(
      TEXT("F=[{0} {1}], C=[{2} {3}]"),
      {FocalLength.X, FocalLength.Y, PrincipalPoint.X, PrincipalPoint.Y});
  FPlatformApplicationMisc::ClipboardCopy(*CameraParameterString);
  return FReply::Handled();
}

FReply FGTImageGeneratorComponentDetails::GenerateStereoCameraParameters() {
  TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
  DetailBuilderCached->GetObjectsBeingCustomized(ObjectsBeingCustomized);

  UGTCameraCalibrationComponent *CalibrationComponent =
      Cast<UGTCameraCalibrationComponent>(ObjectsBeingCustomized[0].Get());

  UGTImageGeneratorBase *GeneratorOne =
      GetImageGeneratorFromReference(CalibrationComponent->CameraOneReference);
  UGTImageGeneratorBase *GeneratorTwo =
      GetImageGeneratorFromReference(CalibrationComponent->CameraTwoReference);

  if (!GeneratorOne || !GeneratorTwo) {
    return FReply::Handled();
  };

  FVector2D FocalLengthOne;
  FVector2D PrincipalPointOne;
  FVector2D FocalLengthTwo;
  FVector2D PrincipalPointTwo;
  FIntPoint ImageSize;
  FRotator Rotation;
  FVector Translation;
  if (!GeneratorOne->GetStereoCalibrationParameters(
          GeneratorTwo, FocalLengthOne, PrincipalPointOne, FocalLengthTwo,
          PrincipalPointTwo, ImageSize, Rotation, Translation)) {
    return FReply::Handled();
  };

  FString CameraParameterString =
      FString::Format(TEXT("F1=[{0} {1}], C1=[{2} {3}]\nF2=[{4} {5}], C2=[{6} "
                           "{7}]\nImgSize=[{8} "
                           "{9}]\nRotation={10}\nTranslation=[{11} {12} {13}]"),
                      {FocalLengthOne.X, FocalLengthOne.Y, PrincipalPointOne.X,
                       PrincipalPointOne.Y, FocalLengthTwo.X, FocalLengthTwo.Y,
                       PrincipalPointTwo.X, PrincipalPointTwo.Y, ImageSize.X,
                       ImageSize.Y, FRotationMatrix(Rotation).ToString(),
                       Translation.X, Translation.Y, Translation.Z});
  FPlatformApplicationMisc::ClipboardCopy(*CameraParameterString);
  return FReply::Handled();
}

UGTImageGeneratorBase *
FGTImageGeneratorComponentDetails::GetImageGeneratorFromReference(
    const FGTGeneratorReference &GeneratorReference) const {
  TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
  DetailBuilderCached->GetObjectsBeingCustomized(ObjectsBeingCustomized);

  UGTCameraCalibrationComponent *CalibrationComponent =
      Cast<UGTCameraCalibrationComponent>(ObjectsBeingCustomized[0].Get());

  UObject *Outer = CalibrationComponent->GetOuter();
  while (Outer && !Cast<UClass>(Outer)) {
    Outer = Outer->GetOuter();
  }
  UClass *OwnerClass = Cast<UClass>(Outer);

  if (GeneratorReference.ComponentProperty != NAME_None) {
    // Cast the actor class to a UBlueprintGeneratedClass
    const UBlueprintGeneratedClass *ActorBlueprintGeneratedClass =
        Cast<UBlueprintGeneratedClass>(Outer);

    // Use UBrintGeneratedClass->SimpleConstructionScript->GetAllNodes() to get
    // an array of USCS_Nodes
    const TArray<USCS_Node *> &ActorBlueprintNodes =
        ActorBlueprintGeneratedClass->SimpleConstructionScript->GetAllNodes();

    // Iterate through the array looking for the USCS_Node whose ComponentClass
    // matches the component you're looking for
    for (USCS_Node *Node : ActorBlueprintNodes) {
      if (Node->GetVariableName() == GeneratorReference.ComponentProperty) {
        UActorComponent *CDO = Node->ComponentTemplate;
        UGTImageGeneratorBase *Generator = Cast<UGTImageGeneratorBase>(CDO);
        return Generator;
      }
    }
  }

  return nullptr;
}

#endif