// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/AIControllers/AISpaceshipController.h"

#include "Pawns/Spaceship.h"


// Sets default values
AAISpaceshipController::AAISpaceshipController()
{
	
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

void AAISpaceshipController::SetDefaultValues(float DefaultSeed, float DefaultFireDelay)
{
	Seed = DefaultSeed;
	FireDelay = DefaultFireDelay;
	FireCooldown = FireDelay*Seed;
}

// Called when the game starts or when spawned
void AAISpaceshipController::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAISpaceshipController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(ControlledSpaceship)
	{
		// Movements
		ControlledSpaceship->InputMove(FVector2D( Seed,0));
		ControlledSpaceship->InputRotate(FVector2D(Seed));

		// Fire
		if(FireCooldown <= 0)
		{
			ControlledSpaceship->InputFire();
			FireCooldown = FireDelay*Seed;
		}
		else
		{
			FireCooldown-=DeltaTime;
		}
	}
}

void AAISpaceshipController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ControlledSpaceship = Cast<ASpaceship>(InPawn);
}

void AAISpaceshipController::OnUnPossess()
{
	Super::OnUnPossess();
}

