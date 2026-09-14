#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WarbandsPlayerController.generated.h"

class UUserWidget;
class UAnimInstance;

UCLASS(Blueprintable)
class WARBANDS_API AWarbandsPlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    AWarbandsPlayerController();
    UFUNCTION(BlueprintCallable, Category="Warbands|Character")
    void FinishCharacterCreation();
    UFUNCTION(BlueprintCallable, Category="Warbands|Equipment")
    void EquipSword();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Warbands|UI")
    TSubclassOf<UUserWidget> CharacterCreationClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Warbands|Equipment")
    TSubclassOf<AActor> SwordClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Warbands|Equipment")
    TSubclassOf<UAnimInstance> SwordGripAnimationClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Warbands|Equipment")
    FName SwordSocket = TEXT("hand_r");
    // Relative to the hand bone; editable on BP_WarbandsPlayerController.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Warbands|Equipment")
    FTransform SwordGripOffset = FTransform::Identity;
protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
    UPROPERTY(Transient) TObjectPtr<UUserWidget> CreationWidget;
    UPROPERTY(Transient) TObjectPtr<AActor> EquippedSword;
};
