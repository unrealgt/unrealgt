// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/ActorComponent.h>
#include <GameFramework/Actor.h>

#include "GTObjectFilter.generated.h"

class UStaticMesh;
class USkeletalMesh;

USTRUCT(BlueprintType)
struct UNREALGT_API FGTObjectFilter {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, Category = Mesh)
  UStaticMesh *StaticMesh;

  UPROPERTY(EditAnywhere, Category = Mesh)
  USkeletalMesh *SkeletalMesh;

  UPROPERTY(EditAnywhere, Category = Mesh)
  FString WildcardMeshName;

  UPROPERTY(EditAnywhere, Category = Class)
  TSubclassOf<AActor> ActorClass;

  /* Will match for components that have the provided class or Actors that have
   * at least one component with the provided class.*/
  UPROPERTY(EditAnywhere, Category = Class)
  TSubclassOf<UActorComponent> ComponentClass;

  /* This will Match Actors that have the provided Tag or components if their
   * Owner has the provided Tag */
  UPROPERTY(EditAnywhere, Category = Tag)
  FName ActorTag;

  UPROPERTY(EditAnywhere, Category = Tag)
  FName ComponentTag;

  /* This will only match with if the tested actor is the same as the provided
   * instance.*/
  UPROPERTY(EditInstanceOnly, Category = Instance)
  AActor *ActorInstance;

  FGTObjectFilter();

  bool MatchesActor(AActor *Actor) const;

  bool MatchesComponent(UActorComponent *ActorComponent) const;

  friend bool operator==(const FGTObjectFilter &X, const FGTObjectFilter &Y);

  friend uint32 GetTypeHash(const FGTObjectFilter &Key);
};
