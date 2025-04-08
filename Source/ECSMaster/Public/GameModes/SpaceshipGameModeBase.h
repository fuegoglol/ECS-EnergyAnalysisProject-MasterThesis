// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SpaceshipGameModeBase.generated.h"

class AAISpaceshipController;
class ASpaceship;
/**
 * 
 */
UCLASS()
class ECSMASTER_API ASpaceshipGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Spawn")
	int NumberOfSpaceships = 500;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Spawn")
	int SpawnRadius = 10000;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Spawn")
	TSubclassOf<ASpaceship> SpaceshipClass;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Spawn")
	TSubclassOf<AAISpaceshipController> SpaceshipAIController;
};	
