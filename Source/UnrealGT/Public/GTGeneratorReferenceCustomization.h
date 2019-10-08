#pragma once

#if WITH_EDITOR

#include "IPropertyTypeCustomization.h"

class IPropertyHandle;
class FDetailWidgetRow;
class IDetailChildrenBuilder;
class SToolTip;

class FGTGeneratorReferenceCustomization : public IPropertyTypeCustomization {
public:
  static TSharedRef<IPropertyTypeCustomization> MakeInstance();

  /** IPropertyTypeCustomization interface */
  void CustomizeHeader(
      TSharedRef<IPropertyHandle> InStructPropertyHandle,
      FDetailWidgetRow &HeaderRow,
      IPropertyTypeCustomizationUtils &StructCustomizationUtils) override;

  void CustomizeChildren(
      TSharedRef<IPropertyHandle> InStructPropertyHandle,
      IDetailChildrenBuilder &StructBuilder,
      IPropertyTypeCustomizationUtils &StructCustomizationUtils) override;

  TArray<FName> GetAvailableComponentNames();

  FName GetSelectedComponentName();

  bool IsSelectionValid();

  void GetAvailableComponentNamesForComboBox(
      TArray<TSharedPtr<FString>> &OutComboBoxStrings,
      TArray<TSharedPtr<SToolTip>> &OutToolTips,
      TArray<bool> &OutRestrictedItems);

  FString GetSelectedComponentNameForComboBox();

  void OnComponentNameSelected(const FString &SelectedName);

private:
  TSharedPtr<IPropertyHandle> StructPropertyHandle;

  UClass *OwnerClass;
};

#endif