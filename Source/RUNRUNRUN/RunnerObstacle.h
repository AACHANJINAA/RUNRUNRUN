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
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runner")
	TObjectPtr<class UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runner")
	TObjectPtr<UStaticMeshComponent> ObstacleMesh;

	UFUNCTION()
	void HandleCollisionBoxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void HandleCollisionBoxHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);
};
