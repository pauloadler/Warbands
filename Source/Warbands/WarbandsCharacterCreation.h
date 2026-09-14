#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WarbandsCharacterCreation.generated.h"
class UWidgetSwitcher;

UCLASS(Blueprintable)
class WARBANDS_API UWarbandsCharacterCreation : public UUserWidget
{
    GENERATED_BODY()
protected:
    virtual void NativeOnInitialized() override;
private:
    UPROPERTY(Transient) TObjectPtr<UWidgetSwitcher> Steps;
    bool bRaceSelected = false;
    bool bFinished = false;
    bool StoreIndex(FName Name, int32 Value);
    void SelectRace(int32 Index);
    void SelectClass(int32 Index);
    UFUNCTION() void ChooseHuman();
    UFUNCTION() void ChooseElf();
    UFUNCTION() void ChooseWarrior();
    UFUNCTION() void ChooseMage();
};
