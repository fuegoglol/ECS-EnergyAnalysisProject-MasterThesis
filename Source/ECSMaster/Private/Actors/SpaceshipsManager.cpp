// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SpaceshipsManager.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SapceshipComponents.h"
#include "GameInstances/SpaceGameInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASpaceshipsManager::ASpaceshipsManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#if !UE_SERVER
	// Used to display spaceships
	SpaceshipsSMInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Spaceships"));
	SpaceshipsSMInstances->SetupAttachment(RootComponent);
#endif
}

// Called when the game starts or when spawned
void ASpaceshipsManager::BeginPlay()
{
	Super::BeginPlay();
	
	USpaceGameInstance* SpaceGameInstance = Cast<USpaceGameInstance>( GetGameInstance());
	ECSWorld = SpaceGameInstance->GetECSWorld();

	TArray<FTransform> SpaceshipTransforms;

	// Generate AI spaceships
	for (int32 i = 0; i<NumberOfSpaceships; ++i)
	{
		auto Spaceship = ECSWorld->entity();

		// Spaceship
		Spaceship.set<FSpaceship>({
			i,
			BaseSpaceshipTranslationSpeed,
			BaseSpaceshipRotationSpeed
		});

		// Transform
		FVector Location = FVector(UKismetMathLibrary::RandomIntegerInRange(-SpawnRadius,SpawnRadius));
		FRotator Rotation = FRotator(UKismetMathLibrary::RandomFloatInRange(0,359));
		Spaceship.set<FSpaceshipTransform>({
			Location,
			Rotation
		});
		SpaceshipTransforms.Add(FTransform(Rotation,Location));

		// AI
		Spaceship.set<FSpaceshipAI>({
			UKismetMathLibrary::RandomFloat()
		});
	}
	
#if UE_SERVER || UE_EDITOR
	
	// Movement system for AI
	auto move_sys = ECSWorld->system<FSpaceship,FSpaceshipTransform, FSpaceshipAI>()
	.each([this](flecs::iter& it, size_t, FSpaceship& S, FSpaceshipTransform& T, FSpaceshipAI& AI) {
		// Rotation
		T.Rotation.Yaw += S.BaseRotationSpeed;
		T.Rotation.Roll += S.BaseRotationSpeed;

		// Location
		T.Location += T.Rotation.Vector() + S.BaseTranslationSpeed * it.delta_time();

		Client_UpdateSpaceships(S.Id,T.Location,T.Rotation);
	});

#endif

#if !UE_SERVER || UE_EDITOR

	SpaceshipsSMInstances->AddInstances(SpaceshipTransforms,true,true);
	
	SpaceshipMovementUpdateQuery = ECSWorld->query<FSpaceship,FSpaceshipTransform>();

	// Rendering update for clients
	auto rendering_sys = ECSWorld->system<FSpaceship,FSpaceshipTransform>()
	.each([this](flecs::iter& it, size_t, FSpaceship& S, FSpaceshipTransform& T) {

		SpaceshipsSMInstances->UpdateInstanceTransform(S.Id,FTransform(T.Rotation,T.Location));
		
	});
#endif
	
}

void ASpaceshipsManager::Client_UpdateSpaceships_Implementation(int32 Id, FVector_NetQuantize NewLocation, FRotator NewRotation)
{
	flecs::entity ShipToUpdate = SpaceshipMovementUpdateQuery.find([Id](FSpaceship& s, FSpaceshipTransform& T)
	{
		return s.Id = Id;
	});

	if(!ShipToUpdate.is_alive()) return;

	ShipToUpdate.set<FSpaceshipTransform>({
			NewLocation,
			NewRotation
		});
}

void ASpaceshipsManager::Server_AddPlayer_Implementation()
{
	flecs::entity ShipToAttribute = SpaceshipMovementUpdateQuery.find([this](FSpaceship& s, FSpaceshipTransform& T)
	{
		return s.Id = LastFreePlayerId;
	});

	PlayerId = LastFreePlayerId;
	LastFreePlayerId++;

	if(!ShipToAttribute.remove<FSpaceshipAI>())
		UE_LOG(LogTemp, Warning, TEXT("Unable to attribute Player Id"));
}

// Called every frame
void ASpaceshipsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!ECSWorld->progress())
		UE_LOG(LogTemp, Warning, TEXT("Unable to progess ECS World"));
}

void ASpaceshipsManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASpaceshipsManager,PlayerId);
}

