#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "CommonActivatableWidget.h"
#include "WeaponUpgradeWidget.generated.h"
/**
 * 
 */
UCLASS()
class TRENTINOALTOADIGE_API UWeaponUpgradeWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWeaponLevel(int32 NewLevel);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWeaponFailedUpgradeMessage(const FString& Message);
	
	UFUNCTION(BlueprintCallable)
	void UpgradeWeapon();
	
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonButtonBase* StartButton;
	
	virtual UWidget* NativeGetDesiredFocusTarget() const override {return StartButton;}
};
