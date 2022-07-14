#pragma once

#include "CoreMinimal.h"

struct UNREALGT_API GTRandomColorGenerator
{
public:
    GTRandomColorGenerator(int MaxHue = 255);
    FColor GetNextRandomColor();

private:
    const TArray<int> RandomHues;
    int CurrentIndex = 0;
};
