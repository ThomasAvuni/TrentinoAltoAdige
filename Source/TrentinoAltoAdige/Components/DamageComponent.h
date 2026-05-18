// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DamageComponent.generated.h"

UENUM()
enum EHitDirection
{
	HitNone,
	Front,
	Back,
	Side
};

USTRUCT(Blueprintable)
struct FDamage
{
	GENERATED_BODY()
	float DamageAmount = 15;
	bool ShouldDoDamage = true;
	EHitDirection HitDirection;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TRENTINOALTOADIGE_API UDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDamageComponent();

	UFUNCTION(Blueprintable)
	void Heal(float HealAmount);
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const {return MaxHealth;}
	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth() const {return CurrentHealth;}
	UFUNCTION(BlueprintCallable)
	bool GetIsDead() const {return IsDead;}
	UFUNCTION(BlueprintCallable)
	void TakeDamage(FDamage Damage);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
	FOnDeath OnDeath;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageResponse, EHitDirection, HitDirection);
	FOnDamageResponse OnDamageResponse;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	bool IsDead = false;

};
	