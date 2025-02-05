// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpaceshipECS.generated.h"

class ASpaceshipController;
class USpringArmComponent;
class UCameraComponent;
class ASpaceshipsManager;

UCLASS()
class ECSMASTER_API ASpaceshipECS : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASpaceshipECS();

	virtual void Tick(float DeltaSeconds) override;
	
protected:
	
	UFUNCTION(BlueprintCallable)
	void InputMove(FVector2D InputValue);

	UFUNCTION(BlueprintCallable)
	void InputRotate(FVector2D InputValue);

	UFUNCTION(BlueprintCallable)
	void InputFire();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Camera related
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Camera")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Camera")
	TObjectPtr<USpringArmComponent> SpringArm;
	
	TObjectPtr<ASpaceshipController> SpaceshipController;
	TObjectPtr<ASpaceshipsManager> SpaceshipManager;

private:

	float LastMovementInput = false;
	FVector2D LastRotInput = FVector2D::ZeroVector;
};
