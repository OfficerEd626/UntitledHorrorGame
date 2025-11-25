// Copyright Epic Games, Inc. All Rights Reserved.

#include "UntitledHorrorGameGameMode.h"
#include "UntitledHorrorGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUntitledHorrorGameGameMode::AUntitledHorrorGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
