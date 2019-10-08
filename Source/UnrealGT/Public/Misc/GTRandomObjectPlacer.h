// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GTObjectFilter.h"
#include "GameFramework/Actor.h"

#include "GTRandomObjectPlacer.generated.h"

class AStaticMeshActor;
class UBoxComponent;

USTRUCT()
struct UNREALGT_API FGTRandomObjectSpawnInfo {
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  UStaticMesh *Mesh;

  UPROPERTY(EditAnywhere)
  float Weight;

  UPROPERTY(EditAnywhere)
  float Radius;

  UPROPERTY(EditAnywhere)
  bool bShouldAlignToGround;

  UPROPERTY(EditAnywhere)
  bool bRandomYaw;

  UPROPERTY(EditAnywhere)
  TArray<UMaterialInterface *> RandomMaterials;

  UPROPERTY(EditAnywhere)
  int RandomMaterialSlotID;

  FGTRandomObjectSpawnInfo()
      : Mesh(nullptr), Weight(10.f), Radius(140.f), bShouldAlignToGround(true),
        bRandomYaw(true), RandomMaterials(), RandomMaterialSlotID(0) {}
};

UCLASS()
class UNREALGT_API AGTRandomObjectPlacer : public AActor {
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  AGTRandomObjectPlacer();

  virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR

  void
  PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) override;

  void PostEditMove(bool bFinished) override;

  bool ShouldTickIfViewportsOnly() const override;

#endif

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

private:
  UPROPERTY(EditAnywhere, Category = GeneratorSettings)
  FVector2D BoundingBox;

  UPROPERTY(EditAnywhere, Category = GeneratorSettings)
  int GeneratorTriesBeforePointRejection;

  UPROPERTY(EditAnywhere, Category = GeneratorSettings)
  float MaximumHeightDifference;

  UPROPERTY(EditAnywhere, Category = ObjectInfo)
  TArray<FGTRandomObjectSpawnInfo> ObjectSpawnInfos;

  UPROPERTY(EditAnywhere, Category = PlacementSettings)
  bool bPlaceMeshActors;

  UPROPERTY(EditAnywhere, Category = PlacementSettings,
            Meta = (EditCondition = "!bPlaceMeshActors"))
  bool bUseInstancedStaticMesh;

  UPROPERTY(EditAnywhere, Category = PlacementSettings)
  bool bAllowPlaceOnLandscape;

  UPROPERTY(EditAnywhere, Category = PlacementSettings)
  TArray<FGTObjectFilter> AllowPlacementOnActorsThatMatchFilter;

  TArray<TArray<bool>> Grid;

  UPROPERTY()
  float GridCellSize;

  UPROPERTY()
  FIntPoint GridSize;

  UPROPERTY()
  TMap<FVector2D, int> Points;

  UPROPERTY()
  TMap<FVector2D, bool> UsedPoints;

  UPROPERTY()
  TArray<AActor *> MeshActors;

  UPROPERTY()
  UBoxComponent *PreviewBoxComponent;

  void GeneratePoints();

  void SpawnObjects();

  void SpawnInstanceMeshComponents();

  void AddMeshes();

  bool IsValidNewPoint(const FVector2D &Point, float Radius);

  void BlockGrid(const FVector2D &Center, float Radius);

  int GetWeightedRandomSpawnInfoID();
};
