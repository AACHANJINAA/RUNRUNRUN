#include "RunnerTile.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "RunnerObstacle.h"
#include "RunnerSpawner.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

ARunnerTile::ARunnerTile()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	FloorCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("FloorCollision"));
	FloorCollision->SetupAttachment(Root);

	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
	FloorMesh->SetupAttachment(Root);

	float FloorThickness = 40.0f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> RoadMesh(TEXT("/Game/RoadBlockoutKit/Meshes/SM_Road.SM_Road"));
	if (RoadMesh.Succeeded())
	{
		FloorMesh->SetStaticMesh(RoadMesh.Object);

		const FBoxSphereBounds MeshBounds = RoadMesh.Object->GetBounds();
		const float MeshLength = FMath::Max(MeshBounds.BoxExtent.X * 2.0f, 1.0f);
		const float MeshWidth = FMath::Max(MeshBounds.BoxExtent.Y * 2.0f, 1.0f);
		const float MeshHeight = MeshBounds.BoxExtent.Z * 2.0f;
		FloorThickness = FMath::Max(MeshHeight * 0.5f, 40.0f);

		FloorMesh->SetWorldScale3D(FVector(TileLength / MeshLength, TileWidth / MeshWidth, 1.0f));
		FloorMesh->SetRelativeLocation(FVector(TileLength * 0.5f, 0.0f, -MeshHeight * 0.5f));
	}

	FloorCollision->SetBoxExtent(FVector(TileLength * 0.5f, TileWidth * 0.5f, FloorThickness));
	FloorCollision->SetRelativeLocation(FVector(TileLength * 0.5f, 0.0f, -FloorThickness));
	FloorCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	FloorCollision->SetCollisionObjectType(ECC_WorldStatic);
	FloorCollision->SetCollisionResponseToAllChannels(ECR_Block);
	FloorCollision->SetGenerateOverlapEvents(false);

	FloorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EndTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("EndTrigger"));
	EndTrigger->SetupAttachment(Root);
	EndTrigger->SetBoxExtent(FVector(150.0f, TileWidth * 0.5f, 300.0f));
	EndTrigger->SetRelativeLocation(FVector(TileLength - 150.0f, 0.0f, 120.0f));
	EndTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	EndTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	EndTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ARunnerTile::BeginPlay()
{
	Super::BeginPlay();
	EndTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARunnerTile::HandleEndTrigger);
}

void ARunnerTile::InitializeTile(ARunnerSpawner* InSpawner, int32 InTileIndex)
{
	Spawner = InSpawner;
	TileIndex = InTileIndex;

	if (!ObstacleClass)
	{
		ObstacleClass = ARunnerObstacle::StaticClass();
	}

	SpawnObstacles();
}

void ARunnerTile::HandleEndTrigger(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && Cast<ACharacter>(OtherActor) && Spawner)
	{
		Spawner->HandleTilePassed(this);
	}
}

void ARunnerTile::SpawnObstacles()
{
	if (!GetWorld() || !ObstacleClass)
	{
		return;
	}

	TArray<int32> AvailableSlots;
	for (int32 Slot = 1; Slot <= 5; ++Slot)
	{
		const float X = (TileLength / 6.0f) * Slot;
		const bool bIsInsideInitialSafeZone = (TileIndex == 0) && (X <= InitialSafeDistance);
		if (!bIsInsideInitialSafeZone)
		{
			AvailableSlots.Add(Slot);
		}
	}

	if (AvailableSlots.Num() == 0)
	{
		return;
	}

	const int32 Count = FMath::RandRange(ObstaclesPerTileMin, ObstaclesPerTileMax);
	const int32 SpawnCount = FMath::Min(Count, AvailableSlots.Num());

	for (int32 Index = 0; Index < SpawnCount; ++Index)
	{
		const int32 Lane = FMath::RandRange(-1, 1);
		const int32 SlotPickIndex = FMath::RandRange(0, AvailableSlots.Num() - 1);
		const int32 Slot = AvailableSlots[SlotPickIndex];
		AvailableSlots.RemoveAtSwap(SlotPickIndex);

		for (int32 CandidateIndex = AvailableSlots.Num() - 1; CandidateIndex >= 0; --CandidateIndex)
		{
			if (FMath::Abs(AvailableSlots[CandidateIndex] - Slot) < MinimumSlotGap)
			{
				AvailableSlots.RemoveAtSwap(CandidateIndex);
			}
		}

		const float X = (TileLength / 6.0f) * Slot;
		const float Y = Lane * LaneWidth;
		const FVector SpawnLocation = GetActorLocation() + FVector(X, Y, 100.0f);
		const FRotator SpawnRotation = FRotator::ZeroRotator;

		ARunnerObstacle* Obstacle = GetWorld()->SpawnActor<ARunnerObstacle>(ObstacleClass, SpawnLocation, SpawnRotation);
		if (Obstacle)
		{
			SpawnedObstacles.Add(Obstacle);
		}
	}
}
