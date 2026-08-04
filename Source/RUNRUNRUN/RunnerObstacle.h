#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RunnerObstacle.generated.h"

class UStaticMeshComponent;

UCLASS()
class RUNRUNRUN_API ARunnerObstacle : public AActor
{
	GENERATED_BODY()

public:
	ARunnerObstacle();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runner")
	TObjectPtr<UStaticMeshComponent> ObstacleMesh;
};
