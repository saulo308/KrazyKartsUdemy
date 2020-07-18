// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "KrazyKartsUdemyGameMode.h"
#include "KrazyKartsUdemyPawn.h"
#include "KrazyKartsUdemyHud.h"

AKrazyKartsUdemyGameMode::AKrazyKartsUdemyGameMode()
{
	DefaultPawnClass = AKrazyKartsUdemyPawn::StaticClass();
	HUDClass = AKrazyKartsUdemyHud::StaticClass();
}
