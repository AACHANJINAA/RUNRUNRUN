#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RunnerSpawner.generated.h"

class ARunnerTile;

UCLASS()
class RUNRUNRUN_API ARunnerSpawner : public AActor
{
	GENERATED_BODY()

public:
	ARunnerSpawner();

	virtual void BeginPlay() override;

	void HandleTilePassed(ARunnerTile* PassedTile);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner")
	TSubclassOf<ARunnerTile> TileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner")
	int32 InitialTileCount = 6;

private:
	UPROPERTY()
	TArray<TObjectPtr<ARunnerTile>> ActiveTiles;

	int32 NextTileIndex = 0;

	void SpawnNextTile();
	void CleanupOldTiles(int32 PassedTileIndex);
};
