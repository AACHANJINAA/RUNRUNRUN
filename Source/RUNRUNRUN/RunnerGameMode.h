#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RunnerGameMode.generated.h"

class ARunnerSpawner;

UCLASS()
class RUNRUNRUN_API ARunnerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARunnerGameMode();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Runner")
	void UpdateScoreFromDistance(float PlayerDistance);

	UFUNCTION(BlueprintCallable, Category = "Runner")
	void HandlePlayerDeath();

	UFUNCTION(BlueprintCallable, Category = "Runner")
	void TogglePauseState();

	UFUNCTION(BlueprintPure, Category = "Runner")
	float GetCurrentScore() const { return CurrentScore; }

	UFUNCTION(BlueprintPure, Category = "Runner")
	float GetBestScore() const { return BestScore; }

	UFUNCTION(BlueprintPure, Category = "Runner")
	bool IsPlayerDead() const { return bPlayerDead; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner")
	TSubclassOf<ARunnerSpawner> SpawnerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner")
	FVector SpawnerLocation = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner")
	FString SaveSlotName = TEXT("RunnerSave");

private:
	UPROPERTY()
	TObjectPtr<ARunnerSpawner> Spawner;

	float CurrentScore = 0.0f;
	float BestScore = 0.0f;
	bool bPlayerDead = false;
	bool bGamePaused = false;

	void LoadBestScore();
	void SaveBestScore();
};
