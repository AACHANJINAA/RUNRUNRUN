#include "RunnerSpawner.h"

#include "Engine/World.h"
#include "RunnerTile.h"

ARunnerSpawner::ARunnerSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ARunnerSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (!TileClass)
	{
		TileClass = ARunnerTile::StaticClass();
	}

	for (int32 Index = 0; Index < InitialTileCount; ++Index)
	{
		SpawnNextTile();
	}
}

void ARunnerSpawner::HandleTilePassed(ARunnerTile* PassedTile)
{
	if (!PassedTile)
	{
		return;
	}

	SpawnNextTile();
	CleanupOldTiles(PassedTile->GetTileIndex());
}

void ARunnerSpawner::SpawnNextTile()
{
	if (!GetWorld() || !TileClass)
	{
		return;
	}

	float TileLength = 3000.0f;
	if (const ARunnerTile* TileDefaults = TileClass->GetDefaultObject<ARunnerTile>())
	{
		TileLength = TileDefaults->GetTileLength();
	}

	const FVector SpawnLocation = GetActorLocation() + FVector((NextTileIndex * TileLength) - (TileLength * 0.5f), 0.0f, 0.0f);
	ARunnerTile* Tile = GetWorld()->SpawnActor<ARunnerTile>(TileClass, SpawnLocation, FRotator::ZeroRotator);
	if (Tile)
	{
		Tile->InitializeTile(this, NextTileIndex);
		ActiveTiles.Add(Tile);
		++NextTileIndex;
	}
}

void ARunnerSpawner::CleanupOldTiles(int32 PassedTileIndex)
{
	// Keep the current tile and one tile behind it so the player never loses the floor
	// while crossing the end trigger near the front edge of a tile.
	const int32 OldestAllowedTileIndex = FMath::Max(0, PassedTileIndex - 1);

	for (int32 Index = ActiveTiles.Num() - 1; Index >= 0; --Index)
	{
		ARunnerTile* Tile = ActiveTiles[Index];
		if (!Tile)
		{
			ActiveTiles.RemoveAt(Index);
			continue;
		}

		if (Tile->GetTileIndex() < OldestAllowedTileIndex)
		{
			Tile->Destroy();
			ActiveTiles.RemoveAt(Index);
		}
	}
}
