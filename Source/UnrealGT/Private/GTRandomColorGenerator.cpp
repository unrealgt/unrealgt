#include "GTRandomColorGenerator.h"

#include "Math/Color.h"

namespace
{
    TArray<int> MakeSequentialIndexArray(const int FirstIndex, const int LastIndex)
    {
        TArray<int> arr;
        arr.Reserve(LastIndex - FirstIndex + 1);
        for (auto i = FirstIndex; i <= LastIndex; i++)
            arr.Emplace(i);
        return arr;
    }

    void ShuffleArray(TArray<int>& Arr)
    {
        for (int i = 0; i < Arr.Num(); ++i)
        {
            int Index = FMath::RandRange(i, Arr.Num() - 1);
            if (i != Index)
            {
                Arr.Swap(i, Index);
            }
        }
    }

    TArray<int> MakeShuffledIntArray(const int LowestIndex, const int HighestIndex)
    {
        TArray<int> IDs = MakeSequentialIndexArray(LowestIndex, HighestIndex);
        ShuffleArray(IDs);
        return IDs;
    }
}

FGTRandomColorGenerator::FGTRandomColorGenerator(int MaxHue)
    : RandomHues(MakeShuffledIntArray(1, MaxHue))
{
}

FColor FGTRandomColorGenerator::GetNextRandomColor()
{
    const auto Hue = RandomHues[CurrentIndex];
    CurrentIndex = (CurrentIndex + 1) % RandomHues.Num();
    return FLinearColor((float)Hue / (float)255 * 360.f, 0.99f, 0.99f)
        .HSVToLinearRGB()
        .ToFColor(false);
}
