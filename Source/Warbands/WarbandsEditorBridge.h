#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WarbandsEditorBridge.generated.h"

// Editor scripting entry point. No editor dependency is linked into game builds.
UCLASS()
class WARBANDS_API UWarbandsEditorBridge : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="Warbands|Editor")
    static FString PrepareCharacterCreation(UObject* BlueprintAsset);
    UFUNCTION(BlueprintCallable, Category="Warbands|Editor")
    static FString PrepareSwordGrip(UObject* BlueprintAsset, UObject* PoseAsset);
    UFUNCTION(BlueprintCallable, Category="Warbands|Editor")
    static bool SetSwordGripPose(UObject* BlueprintAsset, UObject* PoseAsset);
};
