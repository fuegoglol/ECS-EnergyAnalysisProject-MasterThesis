// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "SpaceshipController.generated.h"

class ASpaceshipsManager;
/**
 * 
 */
UCLASS()
class ECSMASTER_API ASpaceshipController : public APlayerController
{
	GENERATED_BODY()

	ASpaceshipController();
	
public:

	UPROPERTY(Replicated)
	int32 PlayerId = 0;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server,Reliable)
	void AddPlayerInput(float MovementInput, FVector2D RotInput);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TSoftObjectPtr<UInputMappingContext> InputMapping_AZERTY;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TSoftObjectPtr<UInputMappingContext> InputMapping_QWERTZ;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TSoftObjectPtr<UInputMappingContext> InputMapping_QWERTY;

	TObjectPtr<ASpaceshipsManager> SpaceshipManager;
	
};
