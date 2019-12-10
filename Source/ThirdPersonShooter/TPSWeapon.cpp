// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h" //for damage and particle effects
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "ThirdPersonShooter.h"

//for debug draw purposes
int32 DebugDrawWeapons = 0;
//FAutoConsoleVariableRef takes a text, a reference variable, and an event in its constructor
FAutoConsoleVariableRef CVARDrawWeapons = FAutoConsoleVariableRef(
	TEXT("TPS.DebugDrawWeapons"),
	DebugDrawWeapons,
	TEXT("Draw debug weapon line trace"),
	ECVF_Cheat);

// Sets default values
ATPSWeapon::ATPSWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Comp"));
	RootComponent = (USceneComponent*)MeshComp;
	TrailEffectParam = "BeamEnd";
}

// Called when the game starts or when spawned
void ATPSWeapon::BeginPlay()
{
	Super::BeginPlay();
	//MuzzleSocketName = "Muzzle";
}

// Called every frame
void ATPSWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATPSWeapon::StartFire()
{
	GetWorldTimerManager().SetTimer(BulletTimer, this, &ATPSWeapon::Fire, 0.5f, true, 0.0f);
}

void ATPSWeapon::EndFire()
{
	GetWorldTimerManager().ClearTimer(BulletTimer);
}

void ATPSWeapon::Fire()
{
	//Create, or rather, get a reference to an owner. It was originally an actor, later refactored into a pawn
	APawn* MyOwner = Cast<APawn>(GetOwner());

	if (MyOwner)
	{
		FVector EyeLoc;
		FRotator EyeRot;
		MyOwner->GetActorEyesViewPoint(EyeLoc, EyeRot);

		FVector LineEnd = EyeLoc + 10000 * EyeRot.Vector();
		FVector TrailEnd = LineEnd;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = false;
		QueryParams.bReturnPhysicalMaterial = true;

		FHitResult Hit;

		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLoc, LineEnd, WeaponTraceChannel, QueryParams))
		{
			AActor* HitActor = Hit.GetActor();

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Cast <UPhysicalMaterial>(Hit.PhysMaterial));

			UParticleSystem* ImpactEffectToPlay = NULL;
			float DamageToApply = BaseDamage;

			switch (SurfaceType)
			{
			case Flash_Default:
				ImpactEffectToPlay = ImpactEffectBlood;
				break;
			case Flash_Vulnerable:
				ImpactEffectToPlay = ImpactEffectBlood;
				DamageToApply *= DamageMultiplier;
				break;
			case Concrete:
				ImpactEffectToPlay = ImpactEffectConcrete;
				break;
			default:
				ImpactEffectToPlay = ImpactEffectBlood;
				break;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, DamageToApply, EyeRot.Vector(), Hit, MyOwner->GetInstigatorController(), this, DamageType);

			TrailEnd = Hit.ImpactPoint; //gotta read the smoke trail end
			if (ImpactEffectToPlay)
			{
				//the effect that takes place when it hits. Different effects go here
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffectToPlay, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
		}

		FVector MuzzlePosition = MeshComp->GetSocketLocation(MuzzleSocketName);

		if (MuzzleEffect)
		{
			//fires from the muzzle.
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, (USceneComponent*)MeshComp, MuzzleSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
		}

		

		if (TrailEffect) //smoke trail
		{
			UParticleSystemComponent* TrailComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TrailEffect, MuzzlePosition);

			if (TrailComp)
			{
				TrailComp->SetVectorParameter(TrailEffectParam, TrailEnd);
			}
		}

		if (DebugDrawWeapons > 0)
		{
			DrawDebugLine(GetWorld(), EyeLoc, LineEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}

		// Camera shake
		APlayerController* PlayerController = Cast<APlayerController>(MyOwner->GetController());
		if (PlayerController)
		{
			PlayerController->ClientPlayCameraShake(FireCameraShake);
		}
		UGameplayStatics::SpawnSoundAtLocation(this, ShotSoundFX, MuzzlePosition);
	}


}

