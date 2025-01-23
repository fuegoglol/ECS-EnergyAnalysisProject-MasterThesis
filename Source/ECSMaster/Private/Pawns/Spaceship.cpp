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
	DOREPLIFETIME(ASpaceship,SpaceshipRotation);
}

// Called when the game starts or when spawned
void ASpaceship::BeginPlay()
{
	Super::BeginPlay();

	SpaceshipLocation = GetActorLocation();

	const auto Rot = GetActorRotation();
	SpaceshipRotation = FVector(Rot.Pitch,Rot.Yaw,Rot.Roll);
}

void ASpaceship::Server_Move_Implementation(FVector_NetQuantize Direction)
{
#if UE_SERVER || UE_EDITOR
	SpaceshipLocation += Direction;
#endif
}

void ASpaceship::Server_Rotate_Implementation(FVector_NetQuantize Rotation)
{
#if UE_SERVER || UE_EDITOR
	SpaceshipRotation += Rotation;	
#endif
}

void ASpaceship::InputMove(FVector2D InputValue)
{
#if !UE_SERVER || UE_EDITOR
	const FVector XDir = GetActorForwardVector()*InputValue.X;
	const FVector YDir = GetActorRightVector()*InputValue.Y;
	const FVector DesiredDir = XDir+YDir;

	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	Server_Move(DesiredDir*DeltaTime*MoveSpeed);
#endif
}

void ASpaceship::InputRotate(FVector2D InputValue)
{
#if !UE_SERVER || UE_EDITOR
	const float Pitch = InputValue.X;
	const float Yaw = InputValue.Y;
	const FVector DesiredRot(Pitch,Yaw,0);

	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	Server_Rotate(DesiredRot*DeltaTime*TurnSpeed);
#endif
}

void ASpaceship::OnRep_SpaceshipLocation(FVector_NetQuantize OldLocation)
{
	SetActorLocation(SpaceshipLocation);
}

void ASpaceship::OnRep_SpaceshipRotation(FVector_NetQuantize OldRotation)
{
	SetActorRotation(FRotator(SpaceshipRotation.X,SpaceshipRotation.Y,SpaceshipRotation.Z));
}

// Called to bind functionality to input
void ASpaceship::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


