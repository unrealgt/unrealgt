#include "GTObjectFilter.h"

#include <Components/SkeletalMeshComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/SkeletalMesh.h>
#include <Engine/StaticMesh.h>

FGTObjectFilter::FGTObjectFilter()
    : StaticMesh(nullptr), SkeletalMesh(nullptr), WildcardMeshName(TEXT("")),
      ActorClass(nullptr), ComponentClass(nullptr), ActorTag(), ComponentTag(),
      ActorInstance(nullptr) {}

bool FGTObjectFilter::MatchesActor(AActor *Actor) const {
  if(!IsValid(Actor))
  {
      return false;
  }
    
  if (ActorInstance) {
    return Actor == ActorInstance;
  }

  // If not set we assume a match
  bool bMatchesStaticMesh = !StaticMesh;
  bool bMatchesSkeletalMesh = !SkeletalMesh;
  bool bMatchesWildcardMeshName = WildcardMeshName.IsEmpty();
  bool bMatchesActorClass = !ActorClass;
  bool bMatchesActorTag = ActorTag.IsNone();
  bool bMatchesComponentClass = !ComponentClass;
  bool bMatchesComponentTag = ComponentTag.IsNone();

  TInlineComponentArray<UActorComponent *> ActorComponents;
  Actor->GetComponents(ActorComponents);
  for (UActorComponent *ActorComponent : ActorComponents) {
    if (StaticMesh || !WildcardMeshName.IsEmpty()) {
      UStaticMeshComponent *StaticMeshComponent =
          Cast<UStaticMeshComponent>(ActorComponent);
      if (StaticMeshComponent) {
        if (StaticMesh && StaticMeshComponent->GetStaticMesh() == StaticMesh) {
          bMatchesStaticMesh = true;
        }
        if (!WildcardMeshName.IsEmpty() &&
            StaticMeshComponent->GetStaticMesh()->GetName().MatchesWildcard(
                WildcardMeshName)) {
          bMatchesWildcardMeshName = true;
        }
      }
    }
    if (SkeletalMesh || !WildcardMeshName.IsEmpty()) {
      USkeletalMeshComponent *SkeletalMeshComponent =
          Cast<USkeletalMeshComponent>(ActorComponent);
      if (SkeletalMeshComponent) {
        if (SkeletalMesh &&
            SkeletalMeshComponent->SkeletalMesh == SkeletalMesh) {
          bMatchesSkeletalMesh = true;
        }
        if (!WildcardMeshName.IsEmpty() &&
            SkeletalMeshComponent->SkeletalMesh->GetName().MatchesWildcard(
                WildcardMeshName)) {
          bMatchesWildcardMeshName = true;
        }
      }
    }
    if (ComponentClass) {
      if (ActorComponent->GetClass()->IsChildOf(ComponentClass)) {
        bMatchesComponentClass = true;
      }
    }
    if (!ComponentTag.IsNone()) {
      if (ActorComponent->ComponentHasTag(ComponentTag)) {
        bMatchesComponentTag = true;
      }
    }
  }

  if (ActorClass) {
    if (Actor->GetClass()->IsChildOf(ActorClass)) {
      bMatchesActorClass = true;
    }
  }

  if (!ActorTag.IsNone()) {
    if (Actor->ActorHasTag(ActorTag)) {
      bMatchesActorTag = true;
    }
  }

  return bMatchesStaticMesh && bMatchesSkeletalMesh &&
         bMatchesWildcardMeshName && bMatchesActorClass && bMatchesActorTag &&
         bMatchesComponentClass && bMatchesComponentClass &&
         bMatchesComponentTag;
}

bool FGTObjectFilter::MatchesComponent(UActorComponent *ActorComponent) const {
  // If not set we assume a match
  bool bMatchesStaticMesh = !StaticMesh;
  bool bMatchesSkeletalMesh = !SkeletalMesh;
  bool bMatchesWildcardMeshName = WildcardMeshName.IsEmpty();
  bool bMatchesActorClass = !ActorClass;
  bool bMatchesActorTag = ActorTag.IsNone();
  bool bMatchesComponentClass = !ComponentClass;
  bool bMatchesComponentTag = ComponentTag.IsNone();

  if (StaticMesh || !WildcardMeshName.IsEmpty()) {
    UStaticMeshComponent *StaticMeshComponent =
        Cast<UStaticMeshComponent>(ActorComponent);
    if (StaticMeshComponent) {
      if (StaticMesh && StaticMeshComponent->GetStaticMesh() == StaticMesh) {
        bMatchesStaticMesh = true;
      }
      if (!WildcardMeshName.IsEmpty() &&
          StaticMeshComponent->GetStaticMesh()->IsValidLowLevel() &&
          StaticMeshComponent->GetStaticMesh()->GetName().MatchesWildcard(
              WildcardMeshName)) {
        bMatchesWildcardMeshName = true;
      }
    }
  }
  if (SkeletalMesh || !WildcardMeshName.IsEmpty()) {
    USkeletalMeshComponent *SkeletalMeshComponent =
        Cast<USkeletalMeshComponent>(ActorComponent);
    if (SkeletalMeshComponent) {
      if (SkeletalMesh && SkeletalMeshComponent->SkeletalMesh == SkeletalMesh) {
        bMatchesSkeletalMesh = true;
      }
      if (!WildcardMeshName.IsEmpty() &&
          SkeletalMeshComponent->SkeletalMesh->GetName().MatchesWildcard(
              WildcardMeshName)) {
        bMatchesWildcardMeshName = true;
      }
    }
  }
  if (ComponentClass) {
    if (ActorComponent->GetClass()->IsChildOf(ComponentClass)) {
      bMatchesComponentClass = true;
    }
  }
  if (!ComponentTag.IsNone()) {
    if (ActorComponent->ComponentHasTag(ComponentTag)) {
      bMatchesComponentTag = true;
    }
  }

  if (ActorClass) {
    if (ActorComponent->GetOwner() &&
        ActorComponent->GetOwner()->GetClass()->IsChildOf(ActorClass)) {
      bMatchesActorClass = true;
    }
  }

  if (!ActorTag.IsNone()) {
    if (ActorComponent->GetOwner() &&
        ActorComponent->GetOwner()->ActorHasTag(ActorTag)) {
      bMatchesActorTag = true;
    }
  }

  return bMatchesStaticMesh && bMatchesSkeletalMesh &&
         bMatchesWildcardMeshName && bMatchesActorClass && bMatchesActorTag &&
         bMatchesComponentClass && bMatchesComponentClass &&
         bMatchesComponentTag;
}

bool operator==(const FGTObjectFilter &X, const FGTObjectFilter &Y) {
  // Check pointer eqaulity first for performance
  return X.ActorClass == Y.ActorClass && X.ActorInstance == Y.ActorInstance &&
         X.ComponentClass == Y.ComponentClass && X.StaticMesh == Y.StaticMesh &&
         X.SkeletalMesh == Y.SkeletalMesh && X.ActorTag == Y.ActorTag &&
         X.ActorTag == Y.ActorTag && X.ComponentTag == Y.ComponentTag &&
         X.WildcardMeshName == Y.WildcardMeshName;
}

uint32 GetTypeHash(const FGTObjectFilter &Key) {
  uint32 KeyHash = 0;
  KeyHash = HashCombine(KeyHash, GetTypeHash(Key.ActorClass));
  KeyHash = HashCombine(KeyHash, GetTypeHash(Key.ActorInstance));
  KeyHash = HashCombine(KeyHash, GetTypeHash(Key.ActorTag));
  KeyHash = HashCombine(KeyHash, GetTypeHash(Key.ComponentClass));
  KeyHash = HashCombine(KeyHash, GetTypeHash(Key.ComponentTag));
  KeyHash = HashCombine(KeyHash, GetTypeHash(Key.SkeletalMesh));
  KeyHash = HashCombine(KeyHash, GetTypeHash(Key.StaticMesh));
  KeyHash = HashCombine(KeyHash, GetTypeHash(Key.WildcardMeshName));

  return KeyHash;
}