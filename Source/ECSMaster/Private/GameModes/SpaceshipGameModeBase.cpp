// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/SpaceshipGameModeBase.h"

#include "AIController.h"
#include "Controllers/AIControllers/AISpaceshipController.h"
#include "ECSMaster/ECSMaster.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pawns/Spaceship.h"

void ASpaceshipGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	const FRandomStream RandomStream = UKismetMathLibrary::MakeRandomStream(RANDOM_SEED);

	for (int32 i = 0; i<NumberOfSpaceships; ++i)
	{
		// Spawn Pawn
		const FTransform SpawnTransform(FVector(
				RandomStream.RandRange(-SpawnRadius,SpawnRadius),
				RandomStream.RandRange(-SpawnRadius,SpawnRadius),
				RandomStream.RandRange(-SpawnRadius,SpawnRadius)));
		
		AActor* Spaceship = GetWorld()->SpawnActor(
			SpaceshipClass,
			&SpawnTransform,
			SpawnParameters);
		
		if(!Spaceship) continue;

		APawn* SpaceshipPawn = Cast<APawn>(Spaceship);

		if(!SpaceshipPawn) continue;

		// Spawn AI Controller
		
		AActor* SpaceshipControllerUncasted = GetWorld()->SpawnActor(
			SpaceshipAIController,
			&FTransform::Identity,
			SpawnParameters);
		
		if(!SpaceshipControllerUncasted) continue;
		
		AAISpaceshipController* SpaceshipController = Cast<AAISpaceshipController>(SpaceshipControllerUncasted);

		if(!SpaceshipController) continue;

		// init default values for controller
		const float Seed = UKismetMathLibrary::FClamp(RandomStream.FRand(),0.1,1);
	
		const float FireDelay = RandomStream.FRandRange(5.f,10.f);

		SpaceshipController->SetDefaultValues(Seed,FireDelay);

		// Possess spawned pawn with spawned controller
		SpaceshipController->Possess(SpaceshipPawn);
	}
}
