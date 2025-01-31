// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpaceshipECS.generated.h"

class ASpaceshipsManager;

UCLASS()
class ECSMASTER_API ASpaceshipECS : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASpaceshipECS();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TObjectPtr<ASpaceshipsManager> SpaceshipManager;
};
