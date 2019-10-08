#pragma once

#include "CoreMinimal.h"

#include "GTGeneratorReference.generated.h"

class UGTDataGeneratorComponent;

USTRUCT()
struct UNREALGT_API FGTGeneratorReference {
  GENERATED_BODY()

public:
  UPROPERTY(EditInstanceOnly, Category = Component)
  AActor *OtherActor;

  /** Name of component property to use */
  UPROPERTY(EditAnywhere, Category = Component)
  FName ComponentProperty;

  /** Allows direct setting of first component to constraint. */
  TWeakObjectPtr<UGTDataGeneratorComponent> OverrideComponent;

  FGTGeneratorReference();

  /** Get the actual component pointer from this reference */
  UGTDataGeneratorComponent *GetComponent(AActor *OwningActor) const;

  UGTDataGeneratorComponent *GetComponent(UClass *OwnerClass) const;
};
