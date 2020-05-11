// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/GTRandomObjectPlacer.h"

#include <Components/BoxComponent.h>
#include <Components/InstancedStaticMeshComponent.h>
#include <Components/StaticMeshComponent.h>
#include <DrawDebugHelpers.h>
#include <Engine/StaticMesh.h>
#include <Engine/StaticMeshActor.h>
#include <Engine/World.h>
#include <EngineUtils.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Landscape.h>
#include <Materials/Material.h>

AGTRandomObjectPlacer::AGTRandomObjectPlacer()
    : Super()
    , BoundingBox(200.f, 400.f)
    , GeneratorTriesBeforePointRejection(50)
    , MaximumHeightDifference(10000.f)
    , bPlaceMeshActors(true)
{
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));

    PreviewBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("PreviewBox"));
    PreviewBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    PreviewBoxComponent->SetupAttachment(RootComponent);
    PreviewBoxComponent->SetBoxExtent(FVector(BoundingBox / 2.f, 2.f * MaximumHeightDifference));

    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

void AGTRandomObjectPlacer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    /* for (TPair<FVector2D, int>& Point : Points)
     {
         FGTRandomObjectSpawnInfo ObjectSpawnInfo = ObjectSpawnInfos[Point.Value];

         FHitResult OutHit;

         FVector GridPointInWorldSpace =
     GetActorTransform().TransformPosition(-FVector(BoundingBox / 2.f, 0.f) +
     FVector(Point.Key, 0.f)); GetWorld()->LineTraceSingleByChannel(OutHit,
                                              GridPointInWorldSpace + FVector(0.f,
     0.f, MaximumHeightDifference), GridPointInWorldSpace + FVector(0.f, 0.f,
     -MaximumHeightDifference), ECC_Visibility); if (OutHit.bBlockingHit)
         {
             FColor CircleColor = UsedPoints.Contains(Point.Key) ? FColor::Green :
     FColor::Red;

             OutHit.Location.Z = GetActorLocation().Z + 100.f;
             DrawDebugCircle(GetWorld(),
                             OutHit.Location,
                             ObjectSpawnInfo.Radius,
                             64,
                             CircleColor,
                             false,
                             DeltaTime * 1.5f,
                             0,
                             25.f,
                             FVector(1.f, 0.f, 0.f),
                             FVector(0.f, 1.f, 0.f));
         }
     }*/
}

#if WITH_EDITOR

void AGTRandomObjectPlacer::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    if (GetWorld() && PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
    {
        GeneratePoints();
        SpawnObjects();
        PreviewBoxComponent->SetBoxExtent(
            FVector(BoundingBox / 2.f, 2.f * MaximumHeightDifference));
    }
}

void AGTRandomObjectPlacer::PostEditMove(bool bFinished)
{
    // Force position update of instanced meshes
    if ((GetWorld() && Points.Num() < 1000) || bFinished)
    {
        AddMeshes();
    }
}

bool AGTRandomObjectPlacer::ShouldTickIfViewportsOnly() const
{
    return true;
}

#endif

void AGTRandomObjectPlacer::BeginPlay()
{
    Super::BeginPlay();
}

void AGTRandomObjectPlacer::GeneratePoints()
{
    if (ObjectSpawnInfos.Num() <= 0)
    {
        return;
    }

    Points.Empty();
    Grid.Empty();

    float MinRadius = BIG_NUMBER;
    float MaxRadius = SMALL_NUMBER;
    for (const FGTRandomObjectSpawnInfo& SpawnInfo : ObjectSpawnInfos)
    {
        MinRadius = FMath::Min(MinRadius, SpawnInfo.Radius);
        MaxRadius = FMath::Max(MaxRadius, SpawnInfo.Radius);
    }

    GridCellSize = MinRadius / PI;

    GridSize = FIntPoint(BoundingBox.X / GridCellSize, BoundingBox.Y / GridCellSize);

    Grid.AddZeroed(GridSize.X);
    for (int X = 0; X < GridSize.X; X++)
    {
        Grid[X].AddZeroed(GridSize.Y);
    }

    TArray<FVector2D> SpawnPoints;
    TMap<FVector2D, int> SpawnPointsToSpawnInfoID;

    int FirstSpawnIndex = SpawnPoints.Add(BoundingBox / 2.f);
    SpawnPointsToSpawnInfoID.Add(SpawnPoints[FirstSpawnIndex], GetWeightedRandomSpawnInfoID());

    while (SpawnPoints.Num() > 0)
    {
        int SpawnIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
        FVector2D CurrentSpawnPoint = SpawnPoints[SpawnIndex];
        FGTRandomObjectSpawnInfo CurrentSpawnInfo =
            ObjectSpawnInfos[SpawnPointsToSpawnInfoID[CurrentSpawnPoint]];

        bool SpawnPointGood = false;

        for (int I = 0; I < GeneratorTriesBeforePointRejection; I++)
        {
            float Angle = FMath::FRand() * PI * 2;
            FVector2D Dir(FMath::Sin(Angle), FMath::Cos(Angle));
            int NewObjectSpawnInfoID = GetWeightedRandomSpawnInfoID();
            FGTRandomObjectSpawnInfo NewObjectSpawnInfo =
                ObjectSpawnInfos[GetWeightedRandomSpawnInfoID()];

            float MinDistanceBetweenObjects = CurrentSpawnInfo.Radius + NewObjectSpawnInfo.Radius;

            FVector2D NewPoint =
                CurrentSpawnPoint +
                Dir * FMath::RandRange(MinDistanceBetweenObjects, MinDistanceBetweenObjects * 2.f);

            if (IsValidNewPoint(NewPoint, NewObjectSpawnInfo.Radius))
            {
                SpawnPoints.Add(NewPoint);
                SpawnPointsToSpawnInfoID.Add(NewPoint, NewObjectSpawnInfoID);
                Points.Add(NewPoint, NewObjectSpawnInfoID);
                SpawnPointGood = true;
                BlockGrid(NewPoint, NewObjectSpawnInfo.Radius);
                break;
            }
        }

        if (!SpawnPointGood)
        {
            SpawnPoints.RemoveAt(SpawnIndex);
            SpawnPointsToSpawnInfoID.Remove(CurrentSpawnPoint);
        }
    }
}

void AGTRandomObjectPlacer::SpawnObjects()
{
    ClearInstanceComponents(true);
    ClearInstanceComponents(false);

    for (AActor* MeshActor : MeshActors)
    {
        MeshActor->Destroy();
    }
    MeshActors.Empty();

    for (FGTRandomObjectSpawnInfo& ObjectSpawnInfo : ObjectSpawnInfos)
    {
        if (ObjectSpawnInfo.Mesh)
        {
            ObjectSpawnInfo.RandomMaterials.AddUnique(
                ObjectSpawnInfo.Mesh->GetMaterial(ObjectSpawnInfo.RandomMaterialSlotID));
        }
    }
    if (bUseInstancedStaticMesh)
    {
        SpawnInstanceMeshComponents();
    }
    AddMeshes();
}

void AGTRandomObjectPlacer::SpawnInstanceMeshComponents()
{
    // create a InstancedStaticMeshComponent for each Mesh and Material
    // combination
    for (FGTRandomObjectSpawnInfo& ObjectSpawnInfo : ObjectSpawnInfos)
    {
        if (ObjectSpawnInfo.Mesh)
        {
            for (UMaterialInterface* MaterialOption : ObjectSpawnInfo.RandomMaterials)
            {
                UInstancedStaticMeshComponent* InstancedStaticMeshComponent =
                    NewObject<UInstancedStaticMeshComponent>(this);

                InstancedStaticMeshComponent->RegisterComponent();
                InstancedStaticMeshComponent->AttachToComponent(
                    RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                InstancedStaticMeshComponent->SetStaticMesh(ObjectSpawnInfo.Mesh);
                // Todo make MaterialIndex configurable
                InstancedStaticMeshComponent->SetMaterial(
                    ObjectSpawnInfo.RandomMaterialSlotID, MaterialOption);
                InstancedStaticMeshComponent->SetFlags(RF_Transactional);

                AddInstanceComponent(InstancedStaticMeshComponent);
            }
        }
    }
}

void AGTRandomObjectPlacer::AddMeshes()
{
    UsedPoints.Empty();

    if (bUseInstancedStaticMesh)
    {
        for (UActorComponent* InstancedComponent : GetInstanceComponents())
        {
            UInstancedStaticMeshComponent* InstanceWithMesh =
                Cast<UInstancedStaticMeshComponent>(InstancedComponent);
            if (InstanceWithMesh)
            {
                InstanceWithMesh->ClearInstances();
            }
        }
    }
    else
    {
        ClearInstanceComponents(true);
        ClearInstanceComponents(false);
    }

    for (AActor* MeshActor : MeshActors)
    {
        MeshActor->Destroy();
    }
    MeshActors.Empty();

    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);

    for (TPair<FVector2D, int>& Point : Points)
    {
        FGTRandomObjectSpawnInfo ObjectSpawnInfo = ObjectSpawnInfos[Point.Value];
        if (ObjectSpawnInfo.Mesh)
        {
            FHitResult OutHit;

            FVector GridPointInWorldSpace = GetActorTransform().TransformPosition(
                -FVector(BoundingBox / 2.f, 0.f) + FVector(Point.Key, 0.f));
            GetWorld()->LineTraceSingleByChannel(
                OutHit,
                GridPointInWorldSpace + FVector(0.f, 0.f, MaximumHeightDifference),
                GridPointInWorldSpace + FVector(0.f, 0.f, -MaximumHeightDifference),
                ECC_Visibility,
                CollisionParams);

            if (OutHit.bBlockingHit)
            {
                bool bShouldPlaceActor = false;

                bool bHitAllowedActor =
                    (Cast<ALandscape>(OutHit.Actor) && bAllowPlaceOnLandscape) ||
                    AllowPlacementOnActorsThatMatchFilter.Num() == 0;

                for (const FGTObjectFilter& Objectfilter : AllowPlacementOnActorsThatMatchFilter)
                {
                    if (Objectfilter.MatchesActor(OutHit.Actor.Get()))
                    {
                        bHitAllowedActor = true;
                    }
                }

                if (bHitAllowedActor)
                {
                    TArray<FHitResult> OutHits;
                    FCollisionQueryParams SphereCollisionParams;
                    SphereCollisionParams.AddIgnoredActor(this);
                    bool bSphereTraceHit = GetWorld()->SweepMultiByChannel(
                        OutHits,
                        OutHit.Location + FVector(0.f, 0.f, ObjectSpawnInfo.Radius),
                        OutHit.Location,
                        FQuat::Identity,
                        ECC_Visibility,
                        FCollisionShape::MakeSphere(ObjectSpawnInfo.Radius),
                        CollisionParams);
                    if (bSphereTraceHit)
                    {
                        bool bSphereOnlyHitAllowedActors = true;
                        for (const FHitResult& SphereHit : OutHits)
                        {
                            if (SphereHit.Actor.IsValid())
                            {
                                if (!bAllowPlaceOnLandscape && Cast<ALandscape>(SphereHit.Actor))
                                {
                                    bSphereOnlyHitAllowedActors = false;
                                }
                                if (AllowPlacementOnActorsThatMatchFilter.Num() != 0)
                                {
                                    bool bOneFilterMatched = false;
                                    for (const FGTObjectFilter& ObjectFilter :
                                         AllowPlacementOnActorsThatMatchFilter)
                                    {
                                        if (ObjectFilter.MatchesActor(SphereHit.Actor.Get()))
                                        {
                                            bOneFilterMatched = true;
                                        }
                                    }
                                    if (!bOneFilterMatched)
                                    {
                                        bSphereOnlyHitAllowedActors = false;
                                    }
                                }
                            }
                        }
                        bShouldPlaceActor = bSphereOnlyHitAllowedActors;
                    }
                }

                if (bShouldPlaceActor)
                {
                    UsedPoints.Add(Point.Key, true);

                    FTransform InstanceTransform =
                        RootComponent->GetComponentTransform().Inverse() *
                        FTransform(OutHit.Location);
                    FRotator GroundRotator = FRotator::ZeroRotator;
                    if (ObjectSpawnInfo.bShouldAlignToGround)
                    {
                        GroundRotator =
                            FMatrix(
                                FVector::CrossProduct(GetActorRightVector(), OutHit.ImpactNormal),
                                GetActorRightVector(),
                                OutHit.ImpactNormal,
                                FVector::ZeroVector)
                                .Rotator();
                    }
                    if (ObjectSpawnInfo.bRandomYaw)
                    {
                        GroundRotator.Yaw = FMath::FRandRange(0.f, 360.f);
                    }
                    InstanceTransform.SetRotation(GroundRotator.Quaternion());

                    UMaterialInterface* RandomMaterial =
                        ObjectSpawnInfo.RandomMaterials[FMath::RandRange(
                            0, ObjectSpawnInfo.RandomMaterials.Num() - 1)];

                    if (bUseInstancedStaticMesh)
                    {
                        for (UActorComponent* InstancedComponent : GetInstanceComponents())
                        {
                            UInstancedStaticMeshComponent* InstanceWithMesh =
                                Cast<UInstancedStaticMeshComponent>(InstancedComponent);
                            if (InstanceWithMesh &&
                                InstanceWithMesh->GetStaticMesh() == ObjectSpawnInfo.Mesh &&
                                InstanceWithMesh->GetMaterial(
                                    ObjectSpawnInfo.RandomMaterialSlotID) == RandomMaterial)
                            {
                                InstanceWithMesh->AddInstance(InstanceTransform);
                                break;
                            }
                        }
                    }
                    else if (!bUseInstancedStaticMesh && !bPlaceMeshActors)
                    {
                        UStaticMeshComponent* MeshComponent = NewObject<UStaticMeshComponent>(this);
                        MeshComponent->RegisterComponent();
                        MeshComponent->AttachToComponent(
                            RootComponent,
                            FAttachmentTransformRules::SnapToTargetNotIncludingScale);
                        MeshComponent->SetRelativeLocationAndRotation(
                            InstanceTransform.GetLocation(), InstanceTransform.GetRotation());
                        MeshComponent->SetStaticMesh(ObjectSpawnInfo.Mesh);
                        MeshComponent->SetMaterial(
                            ObjectSpawnInfo.RandomMaterialSlotID, RandomMaterial);
                        AddInstanceComponent(MeshComponent);
                    }
                    else
                    {
                        UChildActorComponent* ChildActorComponent =
                            NewObject<UChildActorComponent>(this);
                        ChildActorComponent->SetChildActorClass(AStaticMeshActor::StaticClass());
                        ChildActorComponent->CreateChildActor();

                        AStaticMeshActor* ChildMeshActor =
                            Cast<AStaticMeshActor>(ChildActorComponent->GetChildActor());

                        // We need to set mobility before register to avoid warnings
                        ChildMeshActor->GetStaticMeshComponent()->Mobility =
                            EComponentMobility::Movable;

                        ChildActorComponent->RegisterComponent();

                        ChildMeshActor->SetActorLocationAndRotation(
                            OutHit.Location, InstanceTransform.GetRotation());
                        ChildMeshActor->GetStaticMeshComponent()->SetStaticMesh(
                            ObjectSpawnInfo.Mesh);
                        ChildMeshActor->GetStaticMeshComponent()->SetMaterial(
                            ObjectSpawnInfo.RandomMaterialSlotID, RandomMaterial);
                        AddInstanceComponent(ChildActorComponent);
                        // AStaticMeshActor* ChildMeshActor =
                        // GetWorld()->SpawnActor<AStaticMeshActor>(OutHit.Location,
                        // InstanceTransform.GetRotation().Rotator());
                        // ChildMeshActor->GetStaticMeshComponent()->SetStaticMesh(ObjectSpawnInfo.Mesh);
                        // ChildMeshActor->GetStaticMeshComponent()->SetMaterial(ObjectSpawnInfo.RandomMaterialSlotID,
                        // RandomMaterial); MeshActors.Add(ChildMeshActor);
                    }
                }
            }
        }
    }
}

bool AGTRandomObjectPlacer::IsValidNewPoint(const FVector2D& Point, float Radius)
{
    FIntPoint GridCenter(Point.X / GridCellSize, Point.Y / GridCellSize);
    int CellsToCheck = Radius / GridCellSize + 1;

    // TODO maybe actually check a circle in the grid not a square
    for (int X = GridCenter.X - CellsToCheck; X <= GridCenter.X + CellsToCheck; X++)
    {
        for (int Y = GridCenter.Y - CellsToCheck; Y <= GridCenter.Y + CellsToCheck; Y++)
        {
            if (X < 0 || Y < 0)
            {
                return false;
            }
            if (X >= GridSize.X || Y >= GridSize.Y)
            {
                return false;
            }
            if (Grid[X][Y])
            {
                return false;
            }
        }
    }

    return true;
}

void AGTRandomObjectPlacer::BlockGrid(const FVector2D& Center, float Radius)
{
    FIntPoint GridCenter(Center.X / GridCellSize, Center.Y / GridCellSize);

    int CellsToBlock = Radius / GridCellSize + 1;

    // TODO maybe actually block a circle in the grid not a square
    for (int X = GridCenter.X - CellsToBlock; X <= GridCenter.X + CellsToBlock; X++)
    {
        for (int Y = GridCenter.Y - CellsToBlock; Y <= GridCenter.Y + CellsToBlock; Y++)
        {
            if (X >= 0 && Y >= 0 && X < GridSize.X && Y < GridSize.Y)
            {
                Grid[X][Y] = true;
            }
        }
    }
}

int AGTRandomObjectPlacer::GetWeightedRandomSpawnInfoID()
{
    float WeightSum = 0.f;
    for (const FGTRandomObjectSpawnInfo& ObjectSpawnInfo : ObjectSpawnInfos)
    {
        WeightSum += ObjectSpawnInfo.Weight;
    }

    float CurrentWeightSum = 0.f;
    float Weight = FMath::RandRange(0.f, WeightSum - 1.f);

    int ID = 0;

    for (const FGTRandomObjectSpawnInfo& ObjectSpawnInfo : ObjectSpawnInfos)
    {
        CurrentWeightSum += ObjectSpawnInfo.Weight;
        if (CurrentWeightSum > Weight)
        {
            return ID;
        }
        ID++;
    }

    // if for whatever reason weighted selection fails just choose a random
    // element
    return FMath::RandRange(0, ObjectSpawnInfos.Num() - 1);
}
