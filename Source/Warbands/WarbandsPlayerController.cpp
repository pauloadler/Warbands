#include "WarbandsPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"

AWarbandsPlayerController::AWarbandsPlayerController() {}

void AWarbandsPlayerController::BeginPlay()
{
    Super::BeginPlay();
    if (!IsLocalController() || !CharacterCreationClass)
        return;
    CreationWidget = CreateWidget<UUserWidget>(this, CharacterCreationClass);
    if (!CreationWidget)
        return;
    CreationWidget->AddToViewport(10);
    FInputModeUIOnly Mode;
    Mode.SetWidgetToFocus(CreationWidget->TakeWidget());
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(Mode);
    bShowMouseCursor = true;
    UE_LOG(LogTemp, Display, TEXT("Warbands: character creation opened"));
}

void AWarbandsPlayerController::OnPossess(APawn *InPawn)
{
    Super::OnPossess(InPawn);
    EquipSword();
}

void AWarbandsPlayerController::EquipSword()
{
    ACharacter *PlayerCharacter = Cast<ACharacter>(GetPawn());
    if (!PlayerCharacter || !SwordClass)
        return;
    USkeletalMeshComponent *Mesh = PlayerCharacter->GetMesh();
    if (!Mesh || !Mesh->DoesSocketExist(SwordSocket))
    {
        UE_LOG(LogTemp, Error, TEXT("Warbands: hand bone/socket %s not found"), *SwordSocket.ToString());
        return;
    }
    if (IsValid(EquippedSword))
    {
        if (EquippedSword->GetAttachParentActor() == PlayerCharacter)
            return;
        EquippedSword->Destroy();
    }
    FActorSpawnParameters Params;
    Params.Owner = PlayerCharacter;
    Params.Instigator = PlayerCharacter;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    EquippedSword = GetWorld()->SpawnActor<AActor>(SwordClass, FTransform::Identity, Params);
    if (!EquippedSword)
        return;
    TArray<UPrimitiveComponent *> Parts;
    EquippedSword->GetComponents(Parts);
    for (UPrimitiveComponent *Part : Parts)
    {
        Part->SetSimulatePhysics(false);
        Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Part->SetMobility(EComponentMobility::Movable);
    }
    if (USceneComponent *Root = EquippedSword->GetRootComponent())
        Root->SetMobility(EComponentMobility::Movable);
    EquippedSword->AttachToComponent(
        Mesh,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        SwordSocket);

    // Ajuste a rotação aqui: Pitch, Yaw, Roll, em graus.
    //                         Pitch(Y), Yaw(Z), Roll(X)
    const FQuat Rotation = FRotator(-90.f, 90.f, 0.f).Quaternion();

    // Tamanho da espada.
    const FVector Scale(0.5f);

    // Posição: Z, Y, X — em relação à mão.
    const FVector PositionOffset(-10.f, 3.f, 0.f);

    // Compensa a distancia entre o pivo do modelo e o centro do cabo.
    const FVector HandlePosition(78.f, 0.f, 0.f);
    const FVector Location =
        PositionOffset - Rotation.RotateVector(HandlePosition * Scale);

    EquippedSword->SetActorRelativeTransform(
        FTransform(Rotation, Location, Scale));

    if (SwordGripAnimationClass)
        Mesh->SetOverridePostProcessAnimBP(SwordGripAnimationClass);

    UE_LOG(LogTemp, Display, TEXT("Warbands: sword attached to %s"), *SwordSocket.ToString());
}

void AWarbandsPlayerController::FinishCharacterCreation()
{
    if (CreationWidget)
        CreationWidget->RemoveFromParent();
    CreationWidget = nullptr;
    SetInputMode(FInputModeGameOnly());
    bShowMouseCursor = false;
}

void AWarbandsPlayerController::EndPlay(const EEndPlayReason::Type Reason)
{
    if (IsValid(EquippedSword))
        EquippedSword->Destroy();
    Super::EndPlay(Reason);
}
