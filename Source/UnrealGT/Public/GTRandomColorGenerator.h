#pragma once

#include "CoreMinimal.h"

struct UNREALGT_API FGTRandomColorGenerator
{
public:
    FGTRandomColorGenerator(int MaxHue = 255);
    FColor GetNextRandomColor();

private:
    const TArray<int> RandomHues;
    int CurrentIndex = 0;
};
