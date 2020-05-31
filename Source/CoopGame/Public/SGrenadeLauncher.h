// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SGrenadeLauncher.generated.h"

class ASGrenade;

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COOPGAME_API ASGrenadeLauncher : public ASWeapon
{
	GENERATED_BODY()

public:
	ASGrenadeLauncher();

protected:
	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	TSubclassOf<ASGrenade> Projectile;

	UPROPERTY(EditDefaultsOnly, Category="Fire")
	float LaunchSpeed = 2000.f;
};
