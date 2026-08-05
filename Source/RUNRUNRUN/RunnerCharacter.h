#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RunnerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UAnimInstance;
class USkeletalMesh;

UCLASS()
class RUNRUNRUN_API ARunnerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARunnerCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION(BlueprintCallable, Category = "Runner")
	void KillCharacter();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runner")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runner")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner|Visual")
	TObjectPtr<USkeletalMesh> CharacterMeshOverride;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner|Visual")
	TSubclassOf<UAnimInstance> CharacterAnimClassOverride;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner|Movement")
	float ForwardSpeed = 1200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner|Movement")
	float LaneWidth = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner|Movement")
	float LaneInterpSpeed = 12.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner|Movement")
	float TurnAngle = 18.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner|Movement")
	float TurnInterpSpeed = 8.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner|Movement")
	float SlideCapsuleHalfHeight = 44.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Runner|Movement")
	float SlideDuration = 0.65f;

private:
	FVector StartLocation = FVector::ZeroVector;
	float DefaultCapsuleHalfHeight = 0.0f;
	int32 TargetLane = 0;
	bool bIsDead = false;
	FTimerHandle SlideTimerHandle;
	FTimerHandle GroundSnapTimerHandle;

	void MoveLeft();
	void MoveRight();
	void StartSlide();
	void StopSlide();
	void SnapToGround();

	UFUNCTION()
	void HandleCapsuleOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void HandleCapsuleHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	bool IsObstacleActor(const AActor* OtherActor) const;
};
