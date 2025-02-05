// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/SpaceshipGameModeECS.h"

#include "Actors/SpaceshipsManager.h"
#include "Kismet/GameplayStatics.h"


void ASpaceshipGameModeECS::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpaceshipGameModeECS::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if(const auto Actor = UGameplayStatics::GetActorOfClass(GetWorld(),ASpaceshipsManager::StaticClass()))
	{
		if(const auto SpaceshipsManager = Cast<ASpaceshipsManager>(Actor))
		{
			SpaceshipsManager->AddPlayer(NewPlayer);
		}
	}
}
