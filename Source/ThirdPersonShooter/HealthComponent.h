// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UHealthComponent*, HealthComp, float, Health, float, DeltaHealth, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);
UCLASS( ClassGroup=(TPS), meta=(BlueprintSpawnableComponent) )
class THIRDPERSONSHOOTER_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();
	float GetHealth() const;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Health")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	//Regeneration Parameters:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Regeneration Parameters")
	float RegenDelay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Regeneration Parameters")
	float RegenRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Regeneration Parameters")
	float RegenAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Regeneration Parameters")
	bool bCanRegen;
	UPROPERTY(BlueprintReadOnly, Category = "Regeneration Parameters")
	bool bRegenerating;
	FTimerHandle RegenTimer;

	UFUNCTION()
	void Regenerate();

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	UFUNCTION()
	bool GetCanRegen() { return (bCanRegen && !bRegenerating); }

	UFUNCTION(BlueprintCallable)
	void StartRegen();
	UFUNCTION(BlueprintCallable)
	void StopRegen();
};
