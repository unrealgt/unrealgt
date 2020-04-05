#include "GTGeneratorReference.h"

#include <GameFramework/Actor.h>

FGTGeneratorReference::FGTGeneratorReference()
    : OtherActor(nullptr)
    , ComponentProperty(NAME_None)
    , OverrideComponent(nullptr)
{
}

UGTDataGeneratorComponent* FGTGeneratorReference::GetComponent(AActor* OwningActor) const
{
    UGTDataGeneratorComponent* Result = nullptr;
    // Component is specified directly, use that
    if (OverrideComponent.IsValid())
    {
        Result = OverrideComponent.Get();
    }
    else
    {
        // Look in Actor if specified, OwningActor if not
        AActor* SearchActor = (OtherActor != nullptr) ? OtherActor : OwningActor;
        if (SearchActor)
        {
            if (ComponentProperty != NAME_None)
            {
                UObjectPropertyBase* ObjProp =
                    FindField<UObjectPropertyBase>(SearchActor->GetClass(), ComponentProperty);
                if (ObjProp != nullptr)
                {
                    // .. and return the component that is there
                    Result = Cast<UGTDataGeneratorComponent>(
                        ObjProp->GetObjectPropertyValue_InContainer(SearchActor));
                }
            }
        }
    }

    return Result;
}
