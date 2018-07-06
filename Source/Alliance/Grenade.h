// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "Grenade.generated.h"

UCLASS()
class ALLIANCE_API AGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrenade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
		UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY()
		UProjectileMovementComponent* PMovementComponent;
	UPROPERTY()
		UParticleSystemComponent* PSystemComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function called when timer ends and grenade explodes
	UFUNCTION()
		void Explode();

private:
	float Dmg;
	float Timer;
	
};
