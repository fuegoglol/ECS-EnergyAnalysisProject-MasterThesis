// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/SpaceshipECS.h"

#include "Actors/SpaceshipsManager.h"
#include "Camera/CameraComponent.h"
#include "Controllers/PlayerControllers/SpaceshipController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASpaceshipECS::ASpaceshipECS()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	bReplicates = false;
}

void ASpaceshipECS::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(SpaceshipManager)
	{
		if(SpaceshipController)
		SetActorTransform(SpaceshipManager->GetSpaceshipTransform(SpaceshipController->PlayerId));
	}

	if(SpaceshipController && (LastMovementInput != 0 || LastRotInput.Length() != 0))
	{
		SpaceshipController->AddPlayerInput(LastMovementInput,LastRotInput);
	}

	LastMovementInput = 0;
	LastRotInput = FVector2D::ZeroVector;
}

void ASpaceshipECS::InputMove(FVector2D InputValue)
{
	LastMovementInput = InputValue.X;
}

void ASpaceshipECS::InputRotate(FVector2D InputValue)
{
	LastRotInput = InputValue;
}

void ASpaceshipECS::InputFire()
{
}

// Called when the game starts or when spawned
void ASpaceshipECS::BeginPlay()
{
	Super::BeginPlay();

	if(const auto SpaceshipControllerUncasted = GetController())
	{
		 SpaceshipController = Cast<ASpaceshipController>(SpaceshipControllerUncasted);
	}

	if(const auto UncastedSpaceshipManager = UGameplayStatics::GetActorOfClass(GetWorld(),ASpaceshipsManager::StaticClass()))
	{
		SpaceshipManager = Cast<ASpaceshipsManager>(UncastedSpaceshipManager);
	}
}

