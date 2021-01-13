// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealGT.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Docking/SDockTab.h"

#if WITH_EDITOR
#include <LevelEditor.h>
#include <PropertyEditorModule.h>
#endif

#include "GTCommands.h"
#include "GTDebugWidget.h"
#include "GTGeneratorReferenceCustomization.h"
#include "Generators/Image/GTImageGeneratorComponentDetails.h"

static const FName TrainerTabName("Trainer");

#define LOCTEXT_NAMESPACE "FUnrealGTModule"

void FUnrealGTModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact
    // timing is specified in the .uplugin file per-module
    FGTCommands::Register();

#if WITH_EDITOR
    PluginCommands = MakeShareable(new FUICommandList);

    PluginCommands->MapAction(
        FGTCommands::Get().OpenPluginWindow,
        FExecuteAction::CreateRaw(this, &FUnrealGTModule::PluginButtonClicked),
        FCanExecuteAction());

    FLevelEditorModule& LevelEditorModule =
        FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

    {
        TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
        MenuExtender->AddMenuExtension(
            "WindowLayout",
            EExtensionHook::After,
            PluginCommands,
            FMenuExtensionDelegate::CreateRaw(this, &FUnrealGTModule::AddMenuExtension));

        LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
    }

    {
        TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
        ToolbarExtender->AddToolBarExtension(
            "Settings",
            EExtensionHook::After,
            PluginCommands,
            FToolBarExtensionDelegate::CreateRaw(this, &FUnrealGTModule::AddToolbarExtension));

        LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
    }

    FGlobalTabmanager::Get()
        ->RegisterNomadTabSpawner(
            TrainerTabName, FOnSpawnTab::CreateRaw(this, &FUnrealGTModule::OnSpawnPluginTab))
        .SetDisplayName(LOCTEXT("FUnrealGTTabTitle", "UnrealGT: Generator Debug"))
        .SetMenuType(ETabSpawnerMenuType::Hidden);

    // Add details customization
    FPropertyEditorModule& PropertyModule =
        FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    // Custom properties
    PropertyModule.RegisterCustomPropertyTypeLayout(
        "GTGeneratorReference",
        FOnGetPropertyTypeCustomizationInstance::CreateStatic(
            &FGTGeneratorReferenceCustomization::MakeInstance));

    // Custom classes
    PropertyModule.RegisterCustomClassLayout(
        "GTCameraCalibrationComponent",
        FOnGetDetailCustomizationInstance::CreateStatic(
            &FGTImageGeneratorComponentDetails::MakeInstance));
#endif
}

void FUnrealGTModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For
    // modules that support dynamic reloading, we call this function before
    // unloading the module.
    FGTCommands::Unregister();

#if WITH_EDITOR
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TrainerTabName);
#endif
}

#if WITH_EDITOR
TSharedRef<SDockTab> FUnrealGTModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
    FText WidgetText = FText::Format(
        LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
        FText::FromString(TEXT("FTrainerModule::OnSpawnPluginTab")),
        FText::FromString(TEXT("Trainer.cpp")));

    // clang-format off
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth(1.f)
            [
                SNew(SGTDebugWidget)
            ]
		];
    // clang-format on
}

void FUnrealGTModule::PluginButtonClicked()
{
    FGlobalTabmanager::Get()->TryInvokeTab(TrainerTabName);
}

void FUnrealGTModule::AddMenuExtension(FMenuBuilder& Builder)
{
    Builder.AddMenuEntry(FGTCommands::Get().OpenPluginWindow);
}

void FUnrealGTModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
    Builder.AddToolBarButton(FGTCommands::Get().OpenPluginWindow);
}
#endif

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUnrealGTModule, UnrealGT)