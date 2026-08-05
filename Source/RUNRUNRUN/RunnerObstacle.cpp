#include "RunnerObstacle.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Math/UnrealMathUtility.h"

ARunnerObstacle::ARunnerObstacle()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);

	ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleMesh"));
	ObstacleMesh->SetupAttachment(CollisionBox);

	static const TCHAR* ObstacleMeshPaths[] =
	{
		TEXT("/Game/RoadBlockoutKit/Meshes/SM_Obstacle_A_Frame_Sign.SM_Obstacle_A_Frame_Sign"),
		TEXT("/Game/RoadBlockoutKit/Meshes/SM_Obstacle_Barricade.SM_Obstacle_Barricade"),
		TEXT("/Game/RoadBlockoutKit/Meshes/SM_Obstacle_Cabin.SM_Obstacle_Cabin"),
		TEXT("/Game/RoadBlockoutKit/Meshes/SM_Obstacle_Chair.SM_Obstacle_Chair"),
		TEXT("/Game/RoadBlockoutKit/Meshes/SM_Obstacle_Cone.SM_Obstacle_Cone"),
		TEXT("/Game/RoadBlockoutKit/Meshes/SM_Obstacle_Light_Post.SM_Obstacle_Light_Post"),
		TEXT("/Game/RoadBlockoutKit/Meshes/SM_Obstacle_MailBox.SM_Obstacle_MailBox"),
		TEXT("/Game/RoadBlockoutKit/Meshes/SM_Obstacle_Pedestrian_Barrier_Fence.SM_Obstacle_Pedestrian_Barrier_Fence"),
		TEXT("/Game/RoadBlockoutKit/Meshes/SM_Obstacle_Plastic_Soda_Crate.SM_Obstacle_Plastic_Soda_Crate"),
		TEXT("/Game/RoadBlockoutKit/Meshes/SM_Obstacle_Steel_Barrel.SM_Obstacle_Steel_Barrel"),
		TEXT("/Game/RoadBlockoutKit/Meshes/SM_Obstacle_Trash_Cans.SM_Obstacle_Trash_Cans"),
		TEXT("/Game/RoadBlockoutKit/Meshes/SMBO_Obstacle_A_Frame_Ladder.SMBO_Obstacle_A_Frame_Ladder")
	};

	const int32 MeshIndex = FMath::RandRange(0, UE_ARRAY_COUNT(ObstacleMeshPaths) - 1);
	if (UStaticMesh* SelectedMesh = LoadObject<UStaticMesh>(nullptr, ObstacleMeshPaths[MeshIndex]))
	{
		ObstacleMesh->SetStaticMesh(SelectedMesh);
		const FBoxSphereBounds Bounds = SelectedMesh->GetBounds();
		CollisionBox->SetBoxExtent(Bounds.BoxExtent.GetAbs());
	}
	else
	{
		CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 100.0f));
	}

	CollisionBox->SetMobility(EComponentMobility::Movable);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionBox->SetGenerateOverlapEvents(true);
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ARunnerObstacle::HandleCollisionBoxOverlap);
	CollisionBox->OnComponentHit.AddDynamic(this, &ARunnerObstacle::HandleCollisionBoxHit);

	ObstacleMesh->SetMobility(EComponentMobility::Movable);
	ObstacleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ObstacleMesh->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));

	Tags.Add(TEXT("Obstacle"));
}

void ARunnerObstacle::BeginPlay()
{
	Super::BeginPlay();

	const FVector Extent = CollisionBox ? CollisionBox->GetScaledBoxExtent() : FVector(50.0f, 50.0f, 50.0f);
	DrawDebugBox(GetWorld(), GetActorLocation(), Extent, FColor::Green, false, 20.0f, 0, 3.0f);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Obstacle spawned: %s | Location=%s | Extent=%s | CollisionEnabled=%d | Overlap=%d"),
		*GetName(),
		*GetActorLocation().ToString(),
		*Extent.ToString(),
		static_cast<int32>(CollisionBox->GetCollisionEnabled()),
		CollisionBox->GetGenerateOverlapEvents() ? 1 : 0);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Green,
			FString::Printf(TEXT("Obstacle Box: %s"), *GetName()));
	}
}

void ARunnerObstacle::HandleCollisionBoxOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	const FString OtherName = OtherActor ? OtherActor->GetName() : TEXT("None");
	UE_LOG(LogTemp, Warning, TEXT("Obstacle overlap: %s hit %s"), *GetName(), *OtherName);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Yellow,
			FString::Printf(TEXT("Obstacle overlap: %s"), *OtherName));
	}
}

void ARunnerObstacle::HandleCollisionBoxHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	const FString OtherName = OtherActor ? OtherActor->GetName() : TEXT("None");
	UE_LOG(LogTemp, Warning, TEXT("Obstacle hit: %s hit %s"), *GetName(), *OtherName);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Orange,
			FString::Printf(TEXT("Obstacle hit: %s"), *OtherName));
	}
}
