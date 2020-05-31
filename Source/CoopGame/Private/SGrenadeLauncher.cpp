// Fill out your copyright notice in the Description page of Project Settings.


#include "SGrenadeLauncher.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "SGrenade.h"

ASGrenadeLauncher::ASGrenadeLauncher()
{
    PrimaryActorTick.bCanEverTick = true;
    MuzzleSocketName = "MuzzleSocket";
}

void ASGrenadeLauncher::Fire()
{
    auto MyOwner = Cast<APawn>(GetOwner());
    if (MyOwner && MuzzleEffect && Projectile)
    {
        FVector EyeLocation;
        FRotator EyeRotation;
        MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
    
        auto ShotDircetion = EyeRotation.Vector();

        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DefaultImpactEffect, EyeLocation, EyeRotation);

        UE_LOG(LogTemp, Warning, TEXT("Launching"));
        auto LaunchedProjectile = GetWorld()->SpawnActor<ASGrenade>(Projectile, MeshComp->GetSocketLocation(MuzzleSocketName), EyeRotation);
        LaunchedProjectile->Launch(LaunchSpeed);

    	APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
	    {
        	if (PC)
        	{
        		PC->ClientPlayCameraShake(FireCamShake);
        	}
	    }
        LastTimeShoot = GetWorld()->GetTimeSeconds();
    }
}