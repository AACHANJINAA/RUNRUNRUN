#include "RunnerGameMode.h"

#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "RunnerCharacter.h"
#include "RunnerPlayerController.h"
#include "RunnerSaveGame.h"
#include "RunnerSpawner.h"

ARunnerGameMode::ARunnerGameMode()
{
	DefaultPawnClass = ARunnerCharacter::StaticClass();
	PlayerControllerClass = ARunnerPlayerController::StaticClass();
	SpawnerClass = ARunnerSpawner::StaticClass();
}

void ARunnerGameMode::BeginPlay()
{
	Super::BeginPlay();

	CurrentScore = 0.0f;
	bPlayerDead = false;
	bGamePaused = false;
	LoadBestScore();

	if (SpawnerClass && GetWorld())
	{
		Spawner = GetWorld()->SpawnActor<ARunnerSpawner>(SpawnerClass, SpawnerLocation, FRotator::ZeroRotator);
	}
}

void ARunnerGameMode::UpdateScoreFromDistance(float PlayerDistance)
{
	if (bPlayerDead)
	{
		return;
	}

	CurrentScore = FMath::Max(0.0f, PlayerDistance / 100.0f);
	BestScore = FMath::Max(BestScore, CurrentScore);
}

void ARunnerGameMode::HandlePlayerDeath()
{
	if (bPlayerDead)
	{
		return;
	}

	bPlayerDead = true;
	SaveBestScore();
}

void ARunnerGameMode::TogglePauseState()
{
	if (bPlayerDead)
	{
		return;
	}

	bGamePaused = !bGamePaused;
	UGameplayStatics::SetGamePaused(this, bGamePaused);
}

void ARunnerGameMode::LoadBestScore()
{
	if (USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0))
	{
		if (const URunnerSaveGame* RunnerSave = Cast<URunnerSaveGame>(SaveGame))
		{
			BestScore = RunnerSave->BestScore;
		}
	}
}

void ARunnerGameMode::SaveBestScore()
{
	URunnerSaveGame* SaveGame = Cast<URunnerSaveGame>(UGameplayStatics::CreateSaveGameObject(URunnerSaveGame::StaticClass()));
	if (!SaveGame)
	{
		return;
	}

	SaveGame->BestScore = FMath::Max(BestScore, CurrentScore);
	UGameplayStatics::SaveGameToSlot(SaveGame, SaveSlotName, 0);
}
