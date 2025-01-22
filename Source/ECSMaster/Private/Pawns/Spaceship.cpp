// Lori LOU MIT License


#include "ECSMaster/Public/Pawns/Spaceship.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASpaceship::ASpaceship()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Cabin = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cabin"));
	Cabin->SetupAttachment(RootComponent);

	bReplicates = true;
}

void ASpaceship::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ASpaceship,SpaceshipLocation);
}

// Called when the game starts or when spawned
void ASpaceship::BeginPlay()
{
	Super::BeginPlay();

	SpaceshipLocation = GetActorLocation();
}

void ASpaceship::Server_Move_Implementation(FVector Direction)
{
#if UE_SERVER || UE_EDITOR
	SpaceshipLocation += Direction;
#if UE_EDITOR
	SetActorLocation(SpaceshipLocation);
#endif
	
#endif
}
void ASpaceship::InputMove(FVector2D InputValue)
{
#if !UE_SERVER || UE_EDITOR
	const FVector XDir = GetActorForwardVector()*InputValue.X;
	const FVector YDir = GetActorRightVector()*InputValue.Y;
	const FVector DesiredDir = XDir+YDir;
	Server_Move(DesiredDir);
#endif
}

void ASpaceship::OnRep_SpaceshipLocation(FVector_NetQuantize OldLocation)
{
	SetActorLocation(SpaceshipLocation);
}

// Called to bind functionality to input
void ASpaceship::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


