// Fill out your copyright notice in the Description page of Project Settings.


#include "SGrenade.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CoopGame/CoopGame.h"

// Sets default values
ASGrenade::ASGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->bAutoActivate = false;
}

// Called when the game starts or when spawned
void ASGrenade::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASGrenade::Launch(float Speed)
{
	ProjectileMovementComp->SetVelocityInLocalSpace(FVector::ForwardVector * Speed);
	ProjectileMovementComp->Activate();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASGrenade::Explode, DetonationTime, false);
}

void ASGrenade::Explode()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), GetActorRotation());
	if (UGameplayStatics::ApplyRadialDamage(this, 40.f, GetActorLocation(), ExplosionRadius, DamageType, TArray<AActor*>(), this, 0, true, COLLISION_WEAPON))
	{
		UE_LOG(LogTemp, Warning, TEXT("BOOM"));
		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.f, 0, 3.f);
	}
	Destroy();
}

// Called every frame
void ASGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

