﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../flecs/flecs.h"
#include "SpaceGameInstance.generated.h"

UENUM()
enum EKeyboardLayout
{
	AZERTY,
	QWERTY,
	QWERTZ
};

/**
 * 
 */
UCLASS()
class ECSMASTER_API USpaceGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	
	virtual void Init() override;

	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable)
	TEnumAsByte<EKeyboardLayout> GetKeyboardLayoutInternal() const {return KeyboardLayout;}

	flecs::world* GetECSWorld() {return &ECSWorld;}

protected:

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EKeyboardLayout> KeyboardLayout;

	flecs::world ECSWorld;
};
