#include "RunnerPlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "RunnerGameMode.h"

void ARunnerPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void ARunnerPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!InputComponent)
	{
		return;
	}

	InputComponent->BindAction(TEXT("PauseGame"), IE_Pressed, this, &ARunnerPlayerController::TogglePauseState);
	InputComponent->BindAction(TEXT("RestartGame"), IE_Pressed, this, &ARunnerPlayerController::RestartRun);
}

void ARunnerPlayerController::TogglePauseState()
{
	if (ARunnerGameMode* GM = GetWorld()->GetAuthGameMode<ARunnerGameMode>())
	{
		GM->TogglePauseState();
	}
}

void ARunnerPlayerController::RestartRun()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), true);
}
