// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShootTargetGameMode.h"
#include "ShootTargetHUD.h"
#include "ShootTargetCharacter.h"
#include "UObject/ConstructorHelpers.h"

AShootTargetGameMode::AShootTargetGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/BP_FPSCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	//HUDClass = AShootTargetHUD::StaticClass();
}
