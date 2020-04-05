
#if WITH_EDITOR

#include "GTGeneratorReferenceCustomization.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include <DetailCategoryBuilder.h>
#include <DetailLayoutBuilder.h>
#include <DetailWidgetRow.h>
#include <Editor.h>
#include <IDetailChildrenBuilder.h>
#include <PropertyCustomizationHelpers.h>
#include <PropertyHandle.h>

#include "Generators/GTDataGeneratorComponent.h"

#define LOCTEXT_NAMESPACE "FGTGeneratorReferenceCustomization"

TSharedRef<IPropertyTypeCustomization> FGTGeneratorReferenceCustomization::MakeInstance()
{
    return MakeShareable(new FGTGeneratorReferenceCustomization());
}

void FGTGeneratorReferenceCustomization::CustomizeHeader(
    TSharedRef<IPropertyHandle> InStructPropertyHandle,
    FDetailWidgetRow& HeaderRow,
    IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
    StructPropertyHandle = InStructPropertyHandle;
    // SNew(STextBlock).Text(LOCTEXT("HelloSlateText", "Hello, Slate!"))
    // StructPropertyHandle->GetChildHandle(TEXT("ComponentOwner"))->CreatePropertyValueWidget()
    // clang-format off
    HeaderRow
        .NameContent()
        [
            InStructPropertyHandle->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MinDesiredWidth(250.0f)
        .MaxDesiredWidth(0.0f)
        [
            SNew(SVerticalBox)
            //+SVerticalBox::Slot()
            //[
            //    StructPropertyHandle->GetChildHandle(TEXT("ComponentOwner"))->CreatePropertyValueWidget()
            //]
            +SVerticalBox::Slot()
            [
                PropertyCustomizationHelpers::MakePropertyComboBox(
                    StructPropertyHandle->GetChildHandle(TEXT("Component")),
                    FOnGetPropertyComboBoxStrings::CreateSP(this, &FGTGeneratorReferenceCustomization::GetAvailableComponentNamesForComboBox),
                    FOnGetPropertyComboBoxValue::CreateSP(this, &FGTGeneratorReferenceCustomization::GetSelectedComponentNameForComboBox),
                    FOnPropertyComboBoxValueSelected::CreateSP(this, &FGTGeneratorReferenceCustomization::OnComponentNameSelected)
                )
            ]
        ];
    // clang-format on
}

void FGTGeneratorReferenceCustomization::CustomizeChildren(
    TSharedRef<IPropertyHandle> InStructPropertyHandle,
    IDetailChildrenBuilder& StructBuilder,
    IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
    // TODO if in instance mode handle via get components

    TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
    StructBuilder.GetParentCategory().GetParentLayout().GetObjectsBeingCustomized(
        ObjectsBeingCustomized);
    UObject* ParentObject = ObjectsBeingCustomized[0].Get();
    UObject* Outer = ParentObject->GetOuter();
    while (Outer && !Cast<UClass>(Outer))
    {
        Outer = Outer->GetOuter();
    }
    OwnerClass = Cast<UClass>(Outer);

    // Display a warning if the referenced component is no longer available
    if (!IsSelectionValid())
    {
        FNotificationInfo Info(FText::Format(
            LOCTEXT("InvalidComponentReference", "Component Reference {0} is no longer valid."),
            FText::FromName(GetSelectedComponentName())));
        Info.Image = FEditorStyle::GetBrush(TEXT("MessageLog.Warning"));
        Info.FadeInDuration = 0.1f;
        Info.FadeOutDuration = 0.5f;
        Info.ExpireDuration = 6.5f;
        Info.bUseThrobber = false;
        Info.bUseSuccessFailIcons = true;
        Info.bUseLargeFont = true;
        Info.bFireAndForget = true;
        FSlateNotificationManager::Get().AddNotification(Info);

        GEditor->PlayEditorSound(
            TEXT("/Engine/EditorSounds/Notifications/"
                 "CompileFailed_Cue.CompileFailed_Cue"));
    }
}

TArray<FName> FGTGeneratorReferenceCustomization::GetAvailableComponentNames()
{
    TArray<FName> Result;
    for (TFieldIterator<UObjectPropertyBase> Prop(OwnerClass); Prop; ++Prop)
    {
        if (Prop->PropertyClass->IsChildOf(UGTDataGeneratorComponent::StaticClass()))
        {
            Result.Add(Prop->GetFName());
        }
    }

    return Result;
}

FName FGTGeneratorReferenceCustomization::GetSelectedComponentName()
{
    FString DisplayString;
    StructPropertyHandle->GetChildHandle(TEXT("ComponentProperty"))
        ->GetValueAsDisplayString(DisplayString);
    return FName(*DisplayString);
}

bool FGTGeneratorReferenceCustomization::IsSelectionValid()
{
    if (GetSelectedComponentName().IsNone() ||
        GetAvailableComponentNames().Contains(GetSelectedComponentName()))
    {
        return true;
    }
    return false;
}

void FGTGeneratorReferenceCustomization::GetAvailableComponentNamesForComboBox(
    TArray<TSharedPtr<FString>>& OutComboBoxStrings,
    TArray<TSharedPtr<SToolTip>>& OutToolTips,
    TArray<bool>& OutRestrictedItems)
{
    // TODO if in instance mode handle via get components
    for (TFieldIterator<UObjectPropertyBase> Prop(OwnerClass); Prop; ++Prop)
    {
        if (Prop->PropertyClass->IsChildOf(UGTDataGeneratorComponent::StaticClass()))
        {
            OutComboBoxStrings.Add(MakeShared<FString>(Prop->GetFName().ToString()));
        }
    }
}

FString FGTGeneratorReferenceCustomization::GetSelectedComponentNameForComboBox()
{
    const auto DisplayString = GetSelectedComponentName().ToString();
    if (IsSelectionValid())
    {
        return DisplayString;
    }

    return FString(TEXT("INVALID REFRENCE ")) + DisplayString;
}

void FGTGeneratorReferenceCustomization::OnComponentNameSelected(const FString& SelectedName)
{
    StructPropertyHandle->GetChildHandle(TEXT("ComponentProperty"))->SetValue(FName(*SelectedName));
}
#undef LOCTEXT_NAMESPACE

#endif