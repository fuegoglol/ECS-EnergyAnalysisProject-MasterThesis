// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpaceRocket.generated.h"

class UProjectileMovementComponent;

UCLASS()
class ECSMASTER_API ASpaceRocket : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpaceRocket();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Mesh")
	TObjectPtr<UStaticMeshComponent> BulletShape;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
};
