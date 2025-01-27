// Lori LOU MIT License


#include "ECSMaster/Public/Pawns/Spaceship.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Projectiles/SpaceRocket.h"


// Sets default values
ASpaceship::ASpaceship()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Cabin = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cabin"));
	Cabin->SetupAttachment(RootComponent);

#if !UE_SERVER || UE_EDITOR

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(Cabin);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	
#endif
	

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
	OnRep_SpaceshipLocation(GetActorLocation());
#endif
}

void ASpaceship::Server_Rotate_Implementation(FVector_NetQuantize Rotation)
{
#if UE_SERVER || UE_EDITOR
	SpaceshipRotation += Rotation;
	OnRep_SpaceshipRotation(SpaceshipRotation - Rotation);
#endif
}

void ASpaceship::Server_Fire_Implementation()
{
#if UE_SERVER || UE_EDITOR
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<ASpaceRocket>(RocketClass,GetActorLocation(),GetActorRotation(),SpawnParameters);
#endif
}

void ASpaceship::InputMove(FVector2D InputValue)
{
	const FVector XDir = GetActorForwardVector()*InputValue.X;
	const FVector YDir = GetActorRightVector()*InputValue.Y;
	const FVector DesiredDir = XDir+YDir;

	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	Server_Move(DesiredDir*DeltaTime*MoveSpeed);
}

void ASpaceship::InputRotate(FVector2D InputValue)
{
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	const float Pitch = InputValue.X;
	const float Roll = InputValue.Y;
	const FVector DesiredRot(Pitch,0,Roll);
	
	Server_Rotate(DesiredRot*DeltaTime*TurnSpeed);
}

void ASpaceship::InputFire()
{
	Server_Fire();
}

void ASpaceship::OnRep_SpaceshipLocation(FVector_NetQuantize OldLocation)
{
	SetActorLocation(SpaceshipLocation);
}

void ASpaceship::OnRep_SpaceshipRotation(const FVector_NetQuantize& OldRotation)
{
	const FVector DeltaRot = OldRotation-SpaceshipRotation;
	AddActorLocalRotation(FRotator(DeltaRot.X,DeltaRot.Y,DeltaRot.Z));
}

// Called to bind functionality to input
void ASpaceship::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


