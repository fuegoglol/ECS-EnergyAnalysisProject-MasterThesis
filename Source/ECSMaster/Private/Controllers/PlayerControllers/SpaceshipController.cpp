// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PlayerControllers/SpaceshipController.h"
#include "EnhancedInputSubsystems.h"
#include "GameInstances/SpaceGameInstance.h"
#include "Kismet/GameplayStatics.h"

void ASpaceshipController::BeginPlay()
{
	Super::BeginPlay();

	const auto GameInstance = UGameplayStatics::GetGameInstance(GetWorld());

	checkf(GameInstance,TEXT("Unable to get GameInstance"));

	const USpaceGameInstance* SpaceGameInstance = Cast<USpaceGameInstance>(GameInstance);
	if(ensure(SpaceGameInstance))
	{
		const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
		if (ensure(LocalPlayer))
		{
			UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			if (ensure(InputSystem))
			{
				switch (SpaceGameInstance->GetKeyboardLayoutInternal())
				{
				case AZERTY:
					if (!InputMapping_AZERTY.IsNull())
					{
						InputSystem->AddMappingContext(InputMapping_AZERTY.LoadSynchronous(),0);
					}
					break;
				case QWERTZ:
					if (!InputMapping_QWERTZ.IsNull())
					{
						InputSystem->AddMappingContext(InputMapping_QWERTZ.LoadSynchronous(),0);
					}
					break;
				default:
					if (!InputMapping_QWERTY.IsNull())
					{
						InputSystem->AddMappingContext(InputMapping_QWERTY.LoadSynchronous(),0);
					}
					break;
				}
			}
		}
	}
}
