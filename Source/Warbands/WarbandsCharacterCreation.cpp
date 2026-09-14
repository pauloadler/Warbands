#include "WarbandsCharacterCreation.h"
#include "WarbandsPlayerController.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Engine/GameInstance.h"
#include "UObject/UnrealType.h"

void UWarbandsCharacterCreation::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    Steps = Cast<UWidgetSwitcher>(WidgetTree->FindWidget(TEXT("WS_Etapas")));
    UButton* Human = Cast<UButton>(WidgetTree->FindWidget(TEXT("BTN_Humano")));
    UButton* Elf = Cast<UButton>(WidgetTree->FindWidget(TEXT("BTN_Elfo")));
    if (!Steps || !Human || !Elf)
    {
        UE_LOG(LogTemp, Error, TEXT("Warbands: WBP requires WS_Etapas, BTN_Humano and BTN_Elfo"));
        return;
    }
    UButton* Warrior = Cast<UButton>(WidgetTree->FindWidget(TEXT("BTN_Guerreiro")));
    UButton* Mage = Cast<UButton>(WidgetTree->FindWidget(TEXT("BTN_Mago")));
    // Preserve the user's Designer race page; supply the second page when absent.
    if (!Warrior && !Mage && Steps->GetChildrenCount() == 1)
    {
        UVerticalBox* Page = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VB_Classe"));
        UTextBlock* Heading = WidgetTree->ConstructWidget<UTextBlock>();
        Heading->SetText(FText::FromString(TEXT("Escolha sua classe")));
        Heading->SetJustification(ETextJustify::Center);
        Page->AddChildToVerticalBox(Heading)->SetPadding(FMargin(24));
        auto AddChoice = [this, Page](FName Name, const TCHAR* Label)
        {
            UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), Name);
            UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>();
            Text->SetText(FText::FromString(Label));
            Button->AddChild(Text);
            UVerticalBoxSlot* Slot = Page->AddChildToVerticalBox(Button);
            Slot->SetPadding(FMargin(24, 12));
            Slot->SetHorizontalAlignment(HAlign_Center);
            return Button;
        };
        Warrior = AddChoice(TEXT("BTN_Guerreiro"), TEXT("Guerreiro"));
        Mage = AddChoice(TEXT("BTN_Mago"), TEXT("Mago"));
        Steps->AddChild(Page);
    }
    if (!Warrior || !Mage)
    {
        UE_LOG(LogTemp, Error, TEXT("Warbands: incomplete class page"));
        return;
    }
    Human->OnClicked.AddUniqueDynamic(this, &ThisClass::ChooseHuman);
    Elf->OnClicked.AddUniqueDynamic(this, &ThisClass::ChooseElf);
    Warrior->OnClicked.AddUniqueDynamic(this, &ThisClass::ChooseWarrior);
    Mage->OnClicked.AddUniqueDynamic(this, &ThisClass::ChooseMage);
    Steps->SetActiveWidgetIndex(0);
}

bool UWarbandsCharacterCreation::StoreIndex(FName Name, int32 Value)
{
    UGameInstance* Instance = GetGameInstance();
    FIntProperty* Property = Instance ? FindFProperty<FIntProperty>(Instance->GetClass(), Name) : nullptr;
    if (!Property)
    {
        UE_LOG(LogTemp, Error, TEXT("Warbands: GameInstance integer %s missing"), *Name.ToString());
        return false;
    }
    Property->SetPropertyValue_InContainer(Instance, Value);
    UE_LOG(LogTemp, Display, TEXT("Warbands: %s=%d"), *Name.ToString(), Value);
    return true;
}
void UWarbandsCharacterCreation::SelectRace(int32 Index)
{
    if (bFinished || bRaceSelected || !Steps || !StoreIndex(TEXT("RaceIndex"), Index)) return;
    bRaceSelected = true;
    Steps->SetActiveWidgetIndex(1);
}
void UWarbandsCharacterCreation::SelectClass(int32 Index)
{
    if (!bRaceSelected || bFinished || !StoreIndex(TEXT("ClassIndex"), Index)) return;
    bFinished = true;
    if (AWarbandsPlayerController* Controller = Cast<AWarbandsPlayerController>(GetOwningPlayer())) Controller->FinishCharacterCreation();
}
void UWarbandsCharacterCreation::ChooseHuman() { SelectRace(0); }
void UWarbandsCharacterCreation::ChooseElf() { SelectRace(1); }
void UWarbandsCharacterCreation::ChooseWarrior() { SelectClass(0); }
void UWarbandsCharacterCreation::ChooseMage() { SelectClass(1); }
