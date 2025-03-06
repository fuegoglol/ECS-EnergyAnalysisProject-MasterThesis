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

	// Used to display bullets
	BulletsSMInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Bullets"));
	BulletsSMInstances->SetupAttachment(RootComponent);
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
			const float Seed = UKismetMathLibrary::FClamp(UKismetMathLibrary::RandomFloat(),0.1,1);
			Spaceship.set<FSpaceshipAI>({
				Seed 
			});

			// Weapon
			const float FireDelay = UKismetMathLibrary::RandomFloatInRange(5,10);
			Spaceship.set<FWeapon>({
				FireDelay,
				FireDelay*Seed
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

	
	
	// AI move sys
	auto move_sys = ECSWorld->system<FSpaceship,FSpaceshipTransform, FSpaceshipAI>()
	.kind(flecs::PreUpdate)
	.each([this](flecs::iter& it, size_t, FSpaceship& S, FSpaceshipTransform& T, FSpaceshipAI& AI) {
		// Rotation
		T.Rotation.Yaw += S.BaseRotationSpeed * AI.Seed * it.delta_time();
		T.Rotation.Roll += S.BaseRotationSpeed * AI.Seed * it.delta_time();
		// Location
		T.Location += T.Rotation.Vector() * S.BaseTranslationSpeed * it.delta_time() * AI.Seed;

		SpaceshipTransforms[S.Id] = FTransform(T.Rotation,T.Location);
	});

	// AI fire sys
	auto fire_sys = ECSWorld->system<FWeapon,FSpaceshipTransform, FSpaceshipAI>()
	.kind(flecs::PreUpdate)
	.each([this](flecs::iter& it, size_t, FWeapon& W, FSpaceshipTransform& T, FSpaceshipAI& AI) {
		// Firing bullet
		if(W.FireCooldown <= 0)
		{
			const auto Bullet = ECSWorld->entity();
			Bullet.set<FBullet>({
				T.Location,
				T.Rotation.Vector(),
				5
			});

			BulletTransforms.Add(FTransform(T.Location));
			
			W.FireCooldown = W.FireDelay*AI.Seed;
		}
		else
		{
			W.FireCooldown-=it.delta_time();
		}
	});

	// Bullet sys
	auto bullet_sys = ECSWorld->system<FBullet>()
	.kind(flecs::OnUpdate)
	.each([this](flecs::iter& it, size_t index, FBullet& B) {
		if(B.Lifetime <= 0)
		{
			if(BulletTransforms.IsValidIndex(index))
				BulletTransforms.RemoveAt(index);
			it.entity(index).destruct();
		}
		else
		{
			if(BulletTransforms.IsValidIndex(index))
			{
				// Location
			B.Location += B.ForwardVector * 10000 * it.delta_time();
			
				BulletTransforms[index] = FTransform(B.Location);

			B.Lifetime -= it.delta_time();
			}
			else
			{
				it.entity(index).destruct();
			}
			
		}
		
		
		
	});

#endif


	
}

void ASpaceshipsManager::Server_AddPlayer_Implementation(AController* PlayerController)
{
#if UE_EDITOR
	if(PlayerController->IsLocalController()) return;
#endif
	
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

		if(BulletTransforms.Num() > 0)
		{
			BulletsSMInstances->ClearInstances();
			BulletsSMInstances->AddInstances(BulletTransforms,false,true);
		}
#endif
		
	}
	else if(SpaceshipsSMInstances->GetInstanceCount() > 0)
	{
		SpaceshipsSMInstances->BatchUpdateInstancesTransforms(
		0,
		SpaceshipTransforms,
		true);

		if(BulletTransforms.Num() > 0)
		{
			BulletsSMInstances->ClearInstances();
			BulletsSMInstances->AddInstances(BulletTransforms,false,true);
		}
	}

	
}

void ASpaceshipsManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASpaceshipsManager,SpaceshipTransforms);
	DOREPLIFETIME(ASpaceshipsManager,BulletTransforms);
}

