#include "WarbandsEditorBridge.h"
#if WITH_EDITOR
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet2/BlueprintEditorUtils.h"
#endif

FString UWarbandsEditorBridge::PrepareCharacterCreation(UObject* BlueprintAsset)
{
#if WITH_EDITOR
    UWidgetBlueprint* Blueprint = Cast<UWidgetBlueprint>(BlueprintAsset);
    if (!Blueprint || !Blueprint->WidgetTree) return TEXT("ERROR: widget blueprint missing");
    UWidgetTree* Tree = Blueprint->WidgetTree;
    FString Summary;
    Tree->ForEachWidget([&Summary](UWidget* Widget) { Summary += Widget->GetName() + TEXT(":") + Widget->GetClass()->GetName() + TEXT("\n"); });
    UWidgetSwitcher* Steps = Cast<UWidgetSwitcher>(Tree->FindWidget(TEXT("WS_Etapas")));
    if (!Steps || !Tree->FindWidget(TEXT("BTN_Humano")) || !Tree->FindWidget(TEXT("BTN_Elfo")))
        return TEXT("ERROR: missing named race controls\n") + Summary;
    Blueprint->Modify();
    Tree->Modify();
    if (Steps->GetChildrenCount() == 1 && !Tree->FindWidget(TEXT("VB_Classe")))
    {
        UVerticalBox* Page = Tree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VB_Classe"));
        UTextBlock* Heading = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TXT_Classe"));
        Heading->SetText(FText::FromString(TEXT("Escolha sua classe")));
        Heading->SetJustification(ETextJustify::Center);
        Page->AddChildToVerticalBox(Heading)->SetPadding(FMargin(24));
        auto AddChoice = [Tree, Page](const TCHAR* Name, const TCHAR* Label)
        {
            UButton* Button = Tree->ConstructWidget<UButton>(UButton::StaticClass(), Name);
            Button->bIsVariable = true;
            UTextBlock* Text = Tree->ConstructWidget<UTextBlock>();
            Text->SetText(FText::FromString(Label));
            Button->AddChild(Text);
            UVerticalBoxSlot* Slot = Page->AddChildToVerticalBox(Button);
            Slot->SetPadding(FMargin(24, 12));
            Slot->SetHorizontalAlignment(HAlign_Center);
        };
        AddChoice(TEXT("BTN_Guerreiro"), TEXT("Guerreiro"));
        AddChoice(TEXT("BTN_Mago"), TEXT("Mago"));
        Steps->AddChild(Page);
    }
    if (!Tree->FindWidget(TEXT("BTN_Guerreiro")) || !Tree->FindWidget(TEXT("BTN_Mago")))
        return TEXT("ERROR: incomplete class page\n") + Summary;
    Steps->SetActiveWidgetIndex(0);
    Tree->ForEachWidget([Blueprint](UWidget* Widget)
    {
        if (!Blueprint->WidgetVariableNameToGuidMap.Contains(Widget->GetFName()))
            Blueprint->WidgetVariableNameToGuidMap.Add(Widget->GetFName(), FGuid::NewGuid());
    });
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
    return TEXT("OK\n") + Summary;
#else
    return TEXT("ERROR: editor only");
#endif
}
