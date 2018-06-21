// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AllianceGameMode.generated.h"

UCLASS(minimalapi)
class AAllianceGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAllianceGameMode();
	virtual void PostLogin(APlayerController * NewPlayer);
	UFUNCTION(Server, Reliable, WithValidation)
	void RespawnSecondPlayer(APlayerController* SecondPlayer);
	void RespawnSecondPlayer_Implementation(APlayerController* SecondPlayer);
	FORCEINLINE bool RespawnSecondPlayer_Validate(APlayerController* SecondPlayer) { return true; }
};



