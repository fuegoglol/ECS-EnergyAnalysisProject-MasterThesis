// Lori LOU MIT License

#pragma once

#include "CoreMinimal.h"

#include "Spaceship.generated.h"

UCLASS()
class ECSMASTER_API ASpaceship : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASpaceship();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Replicates properties
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable,Server,Reliable)
	void Server_Move(FVector_NetQuantize Direction);

	UFUNCTION(BlueprintCallable,Server,Reliable)
	void Server_Rotate(FVector_NetQuantize Rotation);
	
	UFUNCTION(BlueprintCallable)
	void InputMove(FVector2D InputValue);

	UFUNCTION(BlueprintCallable)
	void InputRotate(FVector2D InputValue);
	
private:

	UFUNCTION()
	void OnRep_SpaceshipLocation(FVector_NetQuantize OldLocation);

	UFUNCTION()
	void OnRep_SpaceshipRotation(FVector_NetQuantize OldRotation);


protected:

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Cosmetic")
	TObjectPtr<UStaticMeshComponent> Cabin;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Movements")
	float MoveSpeed = 100.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Movements")
	float TurnSpeed = 100.f;

	UPROPERTY(ReplicatedUsing="OnRep_SpaceshipLocation")
	FVector_NetQuantize SpaceshipLocation;

	UPROPERTY(ReplicatedUsing="OnRep_SpaceshipRotation")
	FVector_NetQuantize SpaceshipRotation;
};
