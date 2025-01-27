// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/SpaceRocket.h"

#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
ASpaceRocket::ASpaceRocket()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BulletShape = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet"));
	BulletShape->SetupAttachment(RootComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Component");

	bReplicates = true;
}

// Called when the game starts or when spawned
void ASpaceRocket::BeginPlay()
{
	Super::BeginPlay();
	
}

