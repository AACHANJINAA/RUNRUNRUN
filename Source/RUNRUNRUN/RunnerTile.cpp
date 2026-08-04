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

	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
	FloorMesh->SetupAttachment(Root);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		FloorMesh->SetStaticMesh(CubeMesh.Object);
	}

	FloorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	FloorMesh->SetCollisionResponseToAllChannels(ECR_Block);
	FloorMesh->SetWorldScale3D(FVector(TileLength / 100.0f, TileWidth / 100.0f, 0.5f));
	FloorMesh->SetRelativeLocation(FVector(TileLength * 0.5f, 0.0f, -50.0f));

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

	const int32 Count = FMath::RandRange(ObstaclesPerTileMin, ObstaclesPerTileMax);
	TSet<int32> UsedSlots;

	for (int32 Index = 0; Index < Count; ++Index)
	{
		const int32 Lane = FMath::RandRange(-1, 1);
		int32 Slot = FMath::RandRange(1, 5);
		while (UsedSlots.Contains(Slot))
		{
			Slot = FMath::RandRange(1, 5);
		}
		UsedSlots.Add(Slot);

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
