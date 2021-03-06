/* Copyright (C) 2018 Ivan Garcia, Juan Jose Corroto and Javier Cordoba - All Rights Reserved
* You may use, distribute and modify this code under the
* terms of the GNU GPLv3 license.
*
* You should have received a copy of the GNU GPLv3 license with
* this file. If not, please write to: ivan.garcia16@alu.uclm.es
*/

#include "EnemySpawner.h"
#include "EngineUtils.h"
#include "EnemyFactory.h"
#include "Enemy.h"

// Sets default values
AEnemySpawner::AEnemySpawner() : Super(), NumberOfMeleeEnemiesToSpawn{ 0 }, NumberOfRangedEnemiesToSpawn{ 0 }, NumberOfTankEnemiesToSpawn{ 0 }, NumberOfWaves{ 0 }, SpawnedWaves{ 0 }, b_SpawnNewWave{ false },
LocationWhereSpawn(FVector(0.f, 0.f, 0.f)), NumberOfAliveEnemies{ 0 }, b_BossHasBeenSpawned{ false }, b_SpawnFirstBoss{ false }, b_SpawnSecondBoss{ false }, b_SpawnThirdBoss{ false }
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	for (TActorIterator<AEnemyFactory> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		EnemyFactory = *ActorItr;
	}
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Spawn a new wave
	if (b_SpawnNewWave && SpawnedWaves < NumberOfWaves && EnemyFactory)
	{
		SpawnedWaves++;
		b_SpawnNewWave = false;
		
		SpawnBosses();
		SpawnEnemies();
	}
}

void AEnemySpawner::SpawnEnemies()
{
	// Spawn Melee enemies
	for (int i = 0; i < NumberOfMeleeEnemiesToSpawn; i++)
	{
		AEnemy* Enemy = EnemyFactory->SpawnAnEnemy(EEnemyType::ET_Melee, LocationWhereSpawn + FVector(100.f * i, 100.f * i, 0.f),
			FRotator(0.f, 0.f, 0.f), GetUniqueID());
		if (Enemy)
		{
			Enemy->OnEnemyDead.AddDynamic(this, &AEnemySpawner::EnemyDestroyed);
			NumberOfAliveEnemies++;
		}
	}

	// Spawn Ranged enemies
	for (int i = 0; i < NumberOfRangedEnemiesToSpawn; i++)
	{
		AEnemy* Enemy = EnemyFactory->SpawnAnEnemy(EEnemyType::ET_Ranged, LocationWhereSpawn + FVector(500.f + 100.f * i, 500.f + 100.f * i, 0.f),
			FRotator(0.f, 0.f, 0.f), GetUniqueID());
		if (Enemy)
		{
			Enemy->OnEnemyDead.AddDynamic(this, &AEnemySpawner::EnemyDestroyed);
			NumberOfAliveEnemies++;
		}
	}

	// Spawn Distance enemies
	for (int i = 0; i < NumberOfTankEnemiesToSpawn; i++)
	{
		AEnemy* Enemy = EnemyFactory->SpawnAnEnemy(EEnemyType::ET_Tank, LocationWhereSpawn + FVector(1000.f + 100.f * i, 1000.f + 100.f * i, 0.f),
			FRotator(0.f, 0.f, 0.f), GetUniqueID());
		if (Enemy)
		{
			Enemy->OnEnemyDead.AddDynamic(this, &AEnemySpawner::EnemyDestroyed);
			NumberOfAliveEnemies++;
		}
	}
}

void AEnemySpawner::SpawnBosses()
{
	// Check if the spawner has to spawn a boss. If it has, spawns it
	AEnemy* Boss = nullptr;
	if (b_SpawnFirstBoss && !b_BossHasBeenSpawned)
	{
		Boss = EnemyFactory->SpawnAnEnemy(EEnemyType::ET_Henkka, LocationWhereSpawn, FRotator(0.f, 0.f, 0.f), GetUniqueID());
	}
	else if (b_SpawnSecondBoss && !b_BossHasBeenSpawned)
	{
		Boss = EnemyFactory->SpawnAnEnemy(EEnemyType::ET_Euronymous, LocationWhereSpawn, FRotator(0.f, 0.f, 0.f), GetUniqueID());
	}
	else if (b_SpawnThirdBoss && !b_BossHasBeenSpawned)
	{
		Boss = EnemyFactory->SpawnAnEnemy(EEnemyType::ET_Shiva, LocationWhereSpawn, FRotator(0.f, 0.f, 0.f), GetUniqueID());
	}
	if (Boss)
	{
		Boss->OnEnemyDead.AddDynamic(this, &AEnemySpawner::BossDestroyed);
		b_BossHasBeenSpawned = true;
	}
}

void AEnemySpawner::EnemyDestroyed(uint32 UniqueID)
{
	if (UniqueID == GetUniqueID())
	{
		NumberOfAliveEnemies--;
		
		if (NumberOfAliveEnemies == 0)
		{
			b_SpawnNewWave = true;

			// When the spawner has finished its work, destroy it
			if (SpawnedWaves >= NumberOfWaves && !b_BossHasBeenSpawned)
			{
				this->Destroy();
			}
		}
	}
}

void AEnemySpawner::BossDestroyed(uint32 UniqueID)
{
	if (UniqueID == GetUniqueID())
	{
		// If there is no enemies, all waves have been spawned and the boss has been spawned (and killed), destroy the spawner
		if (NumberOfAliveEnemies == 0 && SpawnedWaves >= NumberOfWaves && b_BossHasBeenSpawned)
		{
			this->Destroy();
		}
	}
}

