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
    
  FGTGeneratorReference();

  bool IsSet() const;
    
  /** Get the actual component pointer from this reference */
  UGTDataGeneratorComponent *GetComponent(AActor *OwningActor) const;
};
