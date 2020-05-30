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

void ASGrenadeLauncher::BeginPlay()
{
    Super::BeginPlay();
}

void ASGrenadeLauncher::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ASGrenadeLauncher::Fire()
{
    auto MyOwner = GetOwner();
    if (MyOwner && MuzzleEffect && Projectile)
    {
        FVector EyeLocation;
        FRotator EyeRotation;
        MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
    
        auto ShotDircetion = EyeRotation.Vector();

        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, EyeLocation, EyeRotation);

        UE_LOG(LogTemp, Warning, TEXT("Launching"));
        auto LaunchedProjectile = GetWorld()->SpawnActor<ASGrenade>(Projectile, MeshComp->GetSocketLocation(MuzzleSocketName), EyeRotation);
        LaunchedProjectile->Launch(LaunchSpeed);
    }
}