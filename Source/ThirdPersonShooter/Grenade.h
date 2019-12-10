// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grenade.generated.h"

class USoundCue;
UCLASS()
class THIRDPERSONSHOOTER_API AGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrenade();

	//made public to enable getting in launcher's Fire()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade Appearance")
	UStaticMeshComponent* MeshComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion Properties")
	float expDelay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion Properties")
	float expRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion Properties")
	float expBaseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion Properties")
	UParticleSystem* expEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion Properties")
	USoundCue* expSound;

	FTimerHandle expTimer;

	UFUNCTION(BlueprintCallable)
	void Explode();

	//ought there not be a DamageType* of some sort in here?

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
