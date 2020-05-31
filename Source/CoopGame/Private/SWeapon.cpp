// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"

#include "DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraShake.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopGame/CoopGame.h"
#include "Engine/World.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw Debug Lines for Weapon"), ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	BaseDamage = 20.f;
}

void ASWeapon::StartFire()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastTimeShoot > ShootInterval)
	{
		if (bAutomaticFire)
		{
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ASWeapon::Fire, ShootInterval, true, 0);
		}
		else
		{
			Fire();
		}
	}
}

void ASWeapon::EndFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
}

void ASWeapon::BeginPlay()
{
	LastTimeShoot = GetWorld()->GetTimeSeconds()-ShootInterval;
	ShootInterval = 60/RPM;
}

void ASWeapon::Fire()
{
	auto MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		auto ShotDircetion = EyeRotation.Vector();

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor((MyOwner));
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;
		auto TraceEnd = EyeLocation + ShotDircetion * 10000.f;

		auto TracerEndPoint = TraceEnd;
		
		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			auto HitActor = Hit.GetActor();

			auto  SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			auto ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4.0;
			}
			
			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDircetion, Hit, MyOwner->GetInstigatorController(), this, DamageType);
			
			UParticleSystem* SelectedEffects = nullptr;
			
			switch(SurfaceType)
			{
			case SURFACE_FLESHDEFAULT:
			case SURFACE_FLESHVULNERABLE:
				SelectedEffects = FleshImpactEffect;
				break;
			default:
				SelectedEffects = DefaultImpactEffect;
				break;
			}

			if(SelectedEffects)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffects, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			TracerEndPoint = Hit.ImpactPoint;
		}

		if (DebugWeaponDrawing > 0) 
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.f);
		}
		
		PlayFireEffects(TracerEndPoint);
		LastTimeShoot = GetWorld()->GetTimeSeconds();
	}
}

void ASWeapon::PlayFireEffects(FVector &InTraceEnd)
{
	if(MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	if(TracerEffect)
	{
		auto TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MeshComp->GetSocketLocation(MuzzleSocketName));
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, InTraceEnd);
		}
	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	{
		if (MyOwner)
		{
			APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
			{
				if (PC)
				{
					PC->ClientPlayCameraShake(FireCamShake);
				}
			}
		}
	}
	
}
