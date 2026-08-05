#include "RunnerCharacter.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "RunnerGameMode.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

ARunnerCharacter::ARunnerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->JumpZVelocity = 900.0f;
	GetCharacterMovement()->MaxWalkSpeed = ForwardSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	bUseControllerRotationYaw = false;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultRunnerMesh(
		TEXT("/Game/Resource/Runner.Runner"));
	bool bUsingCustomRunnerMesh = false;
	if (DefaultRunnerMesh.Succeeded())
	{
		CharacterMeshOverride = DefaultRunnerMesh.Object;
		bUsingCustomRunnerMesh = true;
	}
	else
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultMannyMesh(
			TEXT("/MoverTests/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"));
		if (DefaultMannyMesh.Succeeded())
		{
			CharacterMeshOverride = DefaultMannyMesh.Object;
		}
	}

	if (CharacterMeshOverride)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshOverride);
	}

	if (bUsingCustomRunnerMesh)
	{
		GetMesh()->SetRelativeLocation(FVector::ZeroVector);
		GetMesh()->SetRelativeRotation(FRotator::ZeroRotator);
	}
	else
	{
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -96.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}

	if (CharacterAnimClassOverride)
	{
		GetMesh()->SetAnimInstanceClass(CharacterAnimClassOverride);
	}

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 650.0f;
	CameraBoom->SetRelativeLocation(FVector(-200.0f, 0.0f, 220.0f));
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->SetWorldRotation(FRotator(-10.0f, 0.0f, 0.0f));
	CameraBoom->bDoCollisionTest = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ARunnerCharacter::HandleCapsuleOverlap);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ARunnerCharacter::HandleCapsuleHit);
}

void ARunnerCharacter::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	DefaultCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	GetWorldTimerManager().SetTimerForNextTick(this, &ARunnerCharacter::SnapToGround);
}

void ARunnerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsDead)
	{
		return;
	}

	AddMovementInput(FVector::ForwardVector, 1.0f);

	const FVector CurrentLocation = GetActorLocation();
	const float TargetY = StartLocation.Y + (TargetLane * LaneWidth);
	const float NewY = FMath::FInterpTo(CurrentLocation.Y, TargetY, DeltaSeconds, LaneInterpSpeed);
	SetActorLocation(FVector(CurrentLocation.X, NewY, CurrentLocation.Z));

	const float LateralAlpha = FMath::Clamp((TargetY - CurrentLocation.Y) / LaneWidth, -1.0f, 1.0f);
	const float DesiredYaw = LateralAlpha * TurnAngle;
	const float NewYaw = FMath::FInterpTo(GetActorRotation().Yaw, DesiredYaw, DeltaSeconds, TurnInterpSpeed);
	SetActorRotation(FRotator(0.0f, NewYaw, 0.0f));

	if (ARunnerGameMode* GM = GetWorld()->GetAuthGameMode<ARunnerGameMode>())
	{
		GM->AddScoreOverTime(DeltaSeconds);
	}
}

void ARunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("MoveLeft"), IE_Pressed, this, &ARunnerCharacter::MoveLeft);
	PlayerInputComponent->BindAction(TEXT("MoveRight"), IE_Pressed, this, &ARunnerCharacter::MoveRight);
	PlayerInputComponent->BindAction(TEXT("Slide"), IE_Pressed, this, &ARunnerCharacter::StartSlide);
}

void ARunnerCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (IsObstacleActor(OtherActor))
	{
		KillCharacter();
	}
}

void ARunnerCharacter::KillCharacter()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	GetCharacterMovement()->DisableMovement();

	if (ARunnerGameMode* GM = GetWorld()->GetAuthGameMode<ARunnerGameMode>())
	{
		GM->HandlePlayerDeath();
	}
}

void ARunnerCharacter::MoveLeft()
{
	TargetLane = FMath::Clamp(TargetLane - 1, -1, 1);
}

void ARunnerCharacter::MoveRight()
{
	TargetLane = FMath::Clamp(TargetLane + 1, -1, 1);
}

void ARunnerCharacter::StartSlide()
{
	if (bIsDead)
	{
		return;
	}

	GetCapsuleComponent()->SetCapsuleHalfHeight(SlideCapsuleHalfHeight);
	GetWorldTimerManager().ClearTimer(SlideTimerHandle);
	GetWorldTimerManager().SetTimer(SlideTimerHandle, this, &ARunnerCharacter::StopSlide, SlideDuration, false);
}

void ARunnerCharacter::StopSlide()
{
	GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCapsuleHalfHeight);
}

void ARunnerCharacter::SnapToGround()
{
	const FVector CurrentLocation = GetActorLocation();
	const FVector TraceStart = CurrentLocation + FVector(0.0f, 0.0f, 300.0f);
	const FVector TraceEnd = CurrentLocation - FVector(0.0f, 0.0f, 2000.0f);

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(RunnerGroundSnap), false, this);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);

	if (bHit)
	{
		const float TargetZ = Hit.ImpactPoint.Z + GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		SetActorLocation(FVector(CurrentLocation.X, CurrentLocation.Y, TargetZ));
		StartLocation = GetActorLocation();
		return;
	}

	if (!GetWorldTimerManager().IsTimerActive(GroundSnapTimerHandle))
	{
		GetWorldTimerManager().SetTimer(GroundSnapTimerHandle, this, &ARunnerCharacter::SnapToGround, 0.05f, false);
	}
}

void ARunnerCharacter::HandleCapsuleOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (IsObstacleActor(OtherActor))
	{
		KillCharacter();
	}
}

void ARunnerCharacter::HandleCapsuleHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (IsObstacleActor(OtherActor))
	{
		KillCharacter();
	}
}

bool ARunnerCharacter::IsObstacleActor(const AActor* OtherActor) const
{
	if (!OtherActor || OtherActor == this || bIsDead)
	{
		return false;
	}

	if (OtherActor->ActorHasTag(TEXT("Obstacle")))
	{
		return true;
	}

	const FString ActorClassName = OtherActor->GetClass() ? OtherActor->GetClass()->GetName() : FString();
	const FString ActorName = OtherActor->GetName();

	return ActorClassName.Contains(TEXT("Obstacle"))
		|| ActorName.Contains(TEXT("Obstacle"));
}
