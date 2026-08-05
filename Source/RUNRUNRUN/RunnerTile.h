#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RunnerTile.generated.h"

class ARunnerSpawner;
class ARunnerObstacle;
class UBoxComponent;
class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class RUNRUNRUN_API ARunnerTile : public AActor
{
	GENERATED_BODY()

public:
	ARunnerTile();

	void InitializeTile(ARunnerSpawner* InSpawner, int32 InTileIndex);
	float GetTileLength() const { return TileLength; }
	int32 GetTileIndex() const { return TileIndex; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runner")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runner")
	TObjectPtr<UStaticMeshComponent> FloorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runner")
	TObjectPtr<UBoxComponent> FloorCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runner")
	TObjectPtr<UBoxComponent> EndTrigger;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner")
	float TileLength = 3000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner")
	float TileWidth = 1200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner")
	float LaneWidth = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner")
	int32 ObstaclesPerTileMin = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner")
	int32 ObstaclesPerTileMax = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner")
	float InitialSafeDistance = 2400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner")
	int32 MinimumSlotGap = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner")
	TSubclassOf<ARunnerObstacle> ObstacleClass;

private:
	UPROPERTY()
	TObjectPtr<ARunnerSpawner> Spawner;

	UPROPERTY()
	TArray<TObjectPtr<ARunnerObstacle>> SpawnedObstacles;

	int32 TileIndex = INDEX_NONE;

	UFUNCTION()
	void HandleEndTrigger(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void SpawnObstacles();
};
