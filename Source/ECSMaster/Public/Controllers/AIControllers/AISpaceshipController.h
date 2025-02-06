// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "AISpaceshipController.generated.h"

class ASpaceship;

UCLASS()
class ECSMASTER_API AAISpaceshipController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAISpaceshipController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TObjectPtr<ASpaceship> ControlledSpaceship;

	double Seed = 1;
	int FireCooldown = 5;
	int FireDelay = 5;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;
};
