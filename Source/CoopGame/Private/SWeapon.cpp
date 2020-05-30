// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"

#include "DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw Debug Lines for Weapon"), ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

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
		auto TraceEnd = EyeLocation + ShotDircetion * 10000.f;

		auto TracerEndPoint = TraceEnd;
		
		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			auto HitActor = Hit.GetActor();
			UGameplayStatics::ApplyPointDamage(HitActor, 20.f, ShotDircetion, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			if(ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			TracerEndPoint = Hit.ImpactPoint;
		}

		if (DebugWeaponDrawing > 0) 
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.f);
		}
		
		PlayFireEffects(TracerEndPoint);
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
