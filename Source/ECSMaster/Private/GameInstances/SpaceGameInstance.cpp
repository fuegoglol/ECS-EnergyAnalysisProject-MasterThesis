// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstances/SpaceGameInstance.h"
#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#endif


void USpaceGameInstance::Init()
{
	Super::Init();
	
	//setup default keyboard as QWERTY
	KeyboardLayout = QWERTY;

#if PLATFORM_WINDOWS

	switch ( PRIMARYLANGID( LOWORD( GetKeyboardLayout( 0 ) ) ) )
	{
	case LANG_FRENCH:
		KeyboardLayout = AZERTY;
		break;
	case LANG_GERMAN:
		KeyboardLayout = QWERTZ;
		break;
	default:
		KeyboardLayout = QWERTY;
		break;
	}
	
#endif
}
