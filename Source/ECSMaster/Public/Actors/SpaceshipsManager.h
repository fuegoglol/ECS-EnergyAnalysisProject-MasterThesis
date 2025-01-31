// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ECSMaster/flecs/flecs.h"
#include "GameFramework/Actor.h"
#include "SpaceshipsManager.generated.h"

struct FSpaceshipTransform;
struct FSpaceship;

UCLASS()
class ECSMASTER_API ASpaceshipsManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpaceshipsManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Client,Reliable)
	void Client_UpdateSpaceships(int32 Id, FVector_NetQuantize NewLocation, FRotator NewRotation);

	UFUNCTION(Server,Reliable)
	void Server_AddPlayer();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;

protected:

	// Used to display spaceships
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Mesh")
	TObjectPtr<UInstancedStaticMeshComponent> SpaceshipsSMInstances;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Spawn")
	int NumberOfSpaceships = 500;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Spawn")
	int SpawnRadius = 10000;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Movement")
	float BaseSpaceshipTranslationSpeed = 500;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Movement")
	float BaseSpaceshipRotationSpeed = 500;

	const flecs::world* ECSWorld;

	flecs::query<FSpaceship,FSpaceshipTransform> SpaceshipMovementUpdateQuery;

	int32 LastFreePlayerId = 0;

	UPROPERTY(Replicated)
	int32 PlayerId = -1;
};
