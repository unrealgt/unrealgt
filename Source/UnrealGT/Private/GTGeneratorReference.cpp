#include "GTGeneratorReference.h"

#include <GameFramework/Actor.h>

FGTGeneratorReference::FGTGeneratorReference()
    : OtherActor(nullptr)
    , ComponentProperty(NAME_None)
{
}

bool FGTGeneratorReference::IsSet() const
{
    return !ComponentProperty.IsNone();
}

UGTDataGeneratorComponent* FGTGeneratorReference::GetComponent(AActor* OwningActor) const
{
    UGTDataGeneratorComponent* Result = nullptr;

    // Look in Actor if specified, OwningActor if not
    AActor* SearchActor = (OtherActor != nullptr) ? OtherActor : OwningActor;
    if (SearchActor)
    {
        if (ComponentProperty != NAME_None)
        {
            FObjectPropertyBase* ObjProp =
                FindFProperty<FObjectPropertyBase>(SearchActor->GetClass(), ComponentProperty);
            if (ObjProp != nullptr)
            {
                // .. and return the component that is there
                Result = Cast<UGTDataGeneratorComponent>(
                    ObjProp->GetObjectPropertyValue_InContainer(SearchActor));
            }
        }
    }

    return Result;
}
