// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Styling/CoreStyle.h"

class UNREALGT_API FGTCommands : public TCommands<FGTCommands>
{
public:
    FGTCommands()
        : TCommands<FGTCommands>(
              TEXT("Trainer"),
              NSLOCTEXT("Contexts", "Trainer", "Trainer Plugin"),
              NAME_None,
              FCoreStyle::Get().GetStyleSetName())
    {
    }

    // TCommands<> interface
    virtual void RegisterCommands() override;

public:
    TSharedPtr<FUICommandInfo> OpenPluginWindow;
};
