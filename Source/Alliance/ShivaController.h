// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AllianceController.h"
#include "ShivaController.generated.h"

/**
 * 
 */
UCLASS()
class ALLIANCE_API AShivaController : public AAllianceController
{
	GENERATED_BODY()
	
public:
	AShivaController();

	virtual void Possess(APawn* InPawn) override;

	UFUNCTION()
		void SensePawn(const TArray<AActor*> &UpdatedActors);
	
};
