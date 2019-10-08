// Fill out your copyright notice in the Description page of Project Settings.

#include "GTCommands.h"

#define LOCTEXT_NAMESPACE "FUnrealGTModule"

void FGTCommands::RegisterCommands() {
  UI_COMMAND(OpenPluginWindow, "UnrealGT", "Bring up Trainer window",
             EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE