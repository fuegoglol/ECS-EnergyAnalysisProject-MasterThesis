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

	UFUNCTION(BlueprintCallable)
	void AddPlayer(AController* PlayerController);

	UFUNCTION(BlueprintCallable)
	void AddPlayerInput(AController* PlayerController, float MovementInput, FVector2D RotationInput);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Server,Reliable)
	void Server_AddPlayer(AController* PlayerController);

	UFUNCTION(Server,Reliable)
	void Server_AddPlayerInput(int32 PlayerId, float MovementInput, FVector2D RotationInput);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;

	FORCEINLINE FTransform GetSpaceshipTransform(int32 Id) { return SpaceshipTransforms[Id];}

protected:

	// Used to display spaceships
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Mesh")
	TObjectPtr<UInstancedStaticMeshComponent> SpaceshipsSMInstances;

	// Used to display bullets
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Mesh")
	TObjectPtr<UInstancedStaticMeshComponent> BulletsSMInstances;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Spawn")
	int NumberOfSpaceships = 500;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Spawn")
	int SpawnRadius = 10000;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Movement")
	float BaseSpaceshipTranslationSpeed = 5000;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Movement")
	float BaseSpaceshipRotationSpeed = 5000;

	const flecs::world* ECSWorld;

	flecs::query<FSpaceship,FSpaceshipTransform> SpaceshipMovementUpdateQuery;

	int32 LastFreePlayerId = 0;

	int32 NumberOfDestroyedBulletDuringFrame = 0;

	UPROPERTY(Replicated)
	TArray<FTransform> SpaceshipTransforms;

	UPROPERTY(Replicated)
	TArray<FTransform> BulletTransforms;
	
};
