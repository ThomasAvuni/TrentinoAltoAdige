#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyPerceptionConfig.generated.h"

USTRUCT(BlueprintType)
struct FEnemyPerceptionSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Sight")
	float SightRadius = 1200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Sight")
	float LoseSightRadius = 1500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Sight")
	float PeripheralAngle = 70.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Hearing")
	float HearingRadius = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	bool bRespondToDamage = true;

	UPROPERTY(EditDefaultsOnly, Category = "General")
	float StimulusMaxAge = 5.0f;
};

UCLASS(BlueprintType)
class TRENTINOALTOADIGE_API UEnemyPerceptionConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	FEnemyPerceptionSettings Settings;
};