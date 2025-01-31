// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/SpaceshipECS.h"

#include "Actors/SpaceshipsManager.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASpaceshipECS::ASpaceshipECS()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ASpaceshipECS::BeginPlay()
{
	Super::BeginPlay();

	if(const auto SpaceshipManagerUncasted = UGameplayStatics::GetActorOfClass(GetWorld(),ASpaceshipsManager::StaticClass()))
	{
		SpaceshipManager = Cast<ASpaceshipsManager>(SpaceshipManagerUncasted);
	}
}

