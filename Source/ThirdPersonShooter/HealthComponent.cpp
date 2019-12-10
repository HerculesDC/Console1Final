// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	Health = MaxHealth;
}


float UHealthComponent::GetHealth() const
{
	return Health;
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Health != MaxHealth) Health = MaxHealth;

	AActor* myOwner = GetOwner();

	if (myOwner)
	{
		myOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
	}
	
}

void UHealthComponent::Regenerate() {
	
	if (bCanRegen) {
		AActor* MyOwner = Cast<AActor>(GetOwner());
		if (MyOwner && Health < MaxHealth) {
			bRegenerating = true;
			this->HandleTakeAnyDamage(MyOwner, -RegenAmount, nullptr, MyOwner->GetInstigatorController(), MyOwner);
		}
		else {
			StopRegen();
		}
	}
}

void UHealthComponent::StartRegen() {
	
	GetWorld()->GetTimerManager().SetTimer(RegenTimer, this, &UHealthComponent::Regenerate, RegenRate, true, RegenDelay);
}

void UHealthComponent::StopRegen() {

	bRegenerating = false;
	GetWorld()->GetTimerManager().ClearTimer(RegenTimer);
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthComponent::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (Damage == 0.0f)
	{
		return;
	}
	else if (Damage > 0)
	{
		StartRegen();
	}

	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Orange, "Health Changed %f" + FString::SanitizeFloat(Health));
}

