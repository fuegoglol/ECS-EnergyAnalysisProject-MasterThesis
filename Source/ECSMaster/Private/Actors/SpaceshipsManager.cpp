// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SpaceshipsManager.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SapceshipComponents.h"
#include "Controllers/PlayerControllers/SpaceshipController.h"
#include "GameInstances/SpaceGameInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASpaceshipsManager::ASpaceshipsManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

#if !UE_SERVER
	// Used to display spaceships
	SpaceshipsSMInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Spaceships"));
	SpaceshipsSMInstances->SetupAttachment(RootComponent);
#endif
}

void ASpaceshipsManager::AddPlayer(AController* PlayerController)
{
	Server_AddPlayer(PlayerController);
}

void ASpaceshipsManager::AddPlayerInput(AController* PlayerController, float MovementInput, FVector2D RotationInput)
{
	if(const auto SpaceshipController = Cast<ASpaceshipController>(PlayerController))
		Server_AddPlayerInput(SpaceshipController->PlayerId,MovementInput,RotationInput);
}

// Called when the game starts or when spawned
void ASpaceshipsManager::BeginPlay()
{
	Super::BeginPlay();
	
	USpaceGameInstance* SpaceGameInstance = Cast<USpaceGameInstance>( GetGameInstance());
	ECSWorld = SpaceGameInstance->GetECSWorld();

	int Id = -1;

	// Generate AI spaceships
	for (int32 i = 0; i<NumberOfSpaceships; ++i)
	{
		FVector Location = FVector::ZeroVector;
		FRotator Rotation = FRotator::ZeroRotator;
		
		if(HasAuthority())
		{
			auto Spaceship = ECSWorld->entity();
			Id+= 1;

			// Spaceship
			Spaceship.set<FSpaceship>({
				Id,
				BaseSpaceshipTranslationSpeed,
				BaseSpaceshipRotationSpeed
			});

			// Transform
			Location = FVector(
				UKismetMathLibrary::RandomIntegerInRange(-SpawnRadius,SpawnRadius),
				UKismetMathLibrary::RandomIntegerInRange(-SpawnRadius,SpawnRadius),
				UKismetMathLibrary::RandomIntegerInRange(-SpawnRadius,SpawnRadius));
			Rotation = FRotator(UKismetMathLibrary::RandomFloatInRange(0,359));
			Spaceship.set<FSpaceshipTransform>({
				Location,
				Rotation
			});
			

			// AI
			Spaceship.set<FSpaceshipAI>({
				UKismetMathLibrary::RandomFloat()
			});
		}
		
		SpaceshipTransforms.Add(FTransform(Rotation,Location));
		
		
	}

	SpaceshipMovementUpdateQuery = ECSWorld->query<FSpaceship,FSpaceshipTransform>();

#if !UE_SERVER || UE_EDITOR

	
	SpaceshipsSMInstances->AddInstances(SpaceshipTransforms,true,true);
	UE_LOG(LogTemp, Warning, TEXT("Created %d Instances"),SpaceshipsSMInstances->GetInstanceCount());
	
		

#endif
	
#if UE_EDITOR
	if(!HasAuthority()) return;
#endif
		
#if UE_SERVER || UE_EDITOR

	
	
	// Movement system for AI
	
	auto move_sys = ECSWorld->system<FSpaceship,FSpaceshipTransform, FSpaceshipAI>()
	.each([this](flecs::iter& it, size_t, FSpaceship& S, FSpaceshipTransform& T, FSpaceshipAI& AI) {
		// Rotation
		T.Rotation.Yaw += S.BaseRotationSpeed * AI.Seed;
		T.Rotation.Roll += S.BaseRotationSpeed * AI.Seed;
		// Location
		T.Location += T.Rotation.Vector() * S.BaseTranslationSpeed * it.delta_time() * AI.Seed;

		SpaceshipTransforms[S.Id] = FTransform(T.Rotation,T.Location);
	});

#endif


	
}

void ASpaceshipsManager::Server_AddPlayer_Implementation(AController* PlayerController)
{
	flecs::entity ShipToAttribute = SpaceshipMovementUpdateQuery.find([this](FSpaceship& s, FSpaceshipTransform& T)
	{
		return s.Id == LastFreePlayerId;
	});

	if(const auto SpaceshipController = Cast<ASpaceshipController>(PlayerController))
	{
		SpaceshipController->PlayerId = LastFreePlayerId;
		LastFreePlayerId++;
	}
	

	if(!ShipToAttribute.remove<FSpaceshipAI>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to attribute Player Id"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("New Player Created with Id : %d"),LastFreePlayerId-1);
	}
}

void ASpaceshipsManager::Server_AddPlayerInput_Implementation(int32 PlayerId, float MovementInput, FVector2d RotationInput)
{
	SpaceshipMovementUpdateQuery.each([this,PlayerId,MovementInput,RotationInput](flecs::iter& it, size_t, FSpaceship& S, FSpaceshipTransform& T)
	{
		if(S.Id == PlayerId)
		{
			// Rotation
			T.Rotation.Yaw += S.BaseRotationSpeed * RotationInput.X;
			T.Rotation.Roll += S.BaseRotationSpeed * RotationInput.Y;
			// Location
			T.Location += T.Rotation.Vector() * S.BaseTranslationSpeed * it.delta_time() * MovementInput;

			SpaceshipTransforms[S.Id] = FTransform(T.Rotation,T.Location);
		}
	});
	
}

// Called every frame
void ASpaceshipsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HasAuthority())
	{
		if(!ECSWorld->progress())
			UE_LOG(LogTemp, Warning, TEXT("Unable to progess ECS World"));

#if UE_EDITOR
		SpaceshipsSMInstances->BatchUpdateInstancesTransforms(
		0,
		SpaceshipTransforms,
		true);
#endif
		
	}
	else if(SpaceshipsSMInstances->GetInstanceCount() > 0)
	{
		SpaceshipsSMInstances->BatchUpdateInstancesTransforms(
		0,
		SpaceshipTransforms,
		true);
	}
}

void ASpaceshipsManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASpaceshipsManager,SpaceshipTransforms);
}

