#include "WarbandsEditorBridge.h"
#if WITH_EDITOR
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimSequence.h"
#include "AnimGraphNode_Root.h"
#include "AnimGraphNode_LinkedInputPose.h"
#include "AnimGraphNode_SequenceEvaluator.h"
#include "AnimGraphNode_LayeredBoneBlend.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphSchema.h"
#include "Kismet2/BlueprintEditorUtils.h"

template<class T> static T* AddGripNode(UEdGraph* Graph, int32 X, int32 Y)
{
    T* Node = NewObject<T>(Graph);
    Graph->AddNode(Node, false, false);
    Node->CreateNewGuid();
    Node->PostPlacedNewNode();
    Node->NodePosX = X;
    Node->NodePosY = Y;
    return Node;
}
#endif

FString UWarbandsEditorBridge::PrepareSwordGrip(UObject* BlueprintAsset, UObject* PoseAsset)
{
#if WITH_EDITOR
    UAnimBlueprint* BP = Cast<UAnimBlueprint>(BlueprintAsset);
    UAnimSequence* Sequence = Cast<UAnimSequence>(PoseAsset);
    if (!BP || !Sequence) return TEXT("ERROR: expected animation blueprint and sequence");
    TArray<UEdGraph*> Graphs;
    BP->GetAllGraphs(Graphs);
    UAnimGraphNode_Root* Root = nullptr;
    for (UEdGraph* Graph : Graphs)
        for (UEdGraphNode* Node : Graph->Nodes)
            if (auto* Candidate = Cast<UAnimGraphNode_Root>(Node)) Root = Candidate;
    if (!Root) return TEXT("ERROR: no animation root");
    UEdGraph* Graph = Root->GetGraph();
    if (Graph->Nodes.Num() != 1) return TEXT("ERROR: only a new empty graph can be initialized");
    BP->Modify();
    Graph->Modify();
    auto* Input = AddGripNode<UAnimGraphNode_LinkedInputPose>(Graph, -600, 0);
    Input->AllocateDefaultPins();
    auto* Pose = AddGripNode<UAnimGraphNode_SequenceEvaluator>(Graph, -600, 200);
    Pose->Node.SetSequence(Sequence);
    Pose->Node.SetExplicitTime(0.f);
    Pose->AllocateDefaultPins();
    auto* Blend = AddGripNode<UAnimGraphNode_LayeredBoneBlend>(Graph, -250, 0);
    Blend->Node.BlendPoses.SetNum(1);
    Blend->Node.BlendWeights.SetNum(1);
    Blend->Node.BlendWeights[0] = 1.f;
    Blend->Node.LayerSetup.SetNum(1);
    for (const TCHAR* Name : {TEXT("thumb_01_r"), TEXT("index_01_r"), TEXT("middle_01_r"), TEXT("ring_01_r"), TEXT("pinky_01_r")})
    {
        FBranchFilter Filter;
        Filter.BoneName = Name;
        Filter.BlendDepth = 0;
        Blend->Node.LayerSetup[0].BranchFilters.Add(Filter);
    }
    Blend->AllocateDefaultPins();
    auto Output = [](UEdGraphNode* Node) -> UEdGraphPin*
    {
        for (UEdGraphPin* Pin : Node->Pins) if (Pin->Direction == EGPD_Output) return Pin;
        return nullptr;
    };
    auto Connect = [Graph](UEdGraphPin* A, UEdGraphPin* B)
    { return A && B && Graph->GetSchema()->TryCreateConnection(A, B); };
    if (!Connect(Output(Input), Blend->FindPin(TEXT("BasePose"))) ||
        !Connect(Output(Pose), Blend->FindPin(TEXT("BlendPoses_0"))) ||
        !Connect(Output(Blend), Root->FindPin(TEXT("Result"))))
    {
        FString Result = TEXT("ERROR: pin connection failed\n");
        for (UEdGraphNode* Node : Graph->Nodes) for (UEdGraphPin* Pin : Node->Pins) Result += Node->GetClass()->GetName() + TEXT(" ") + Pin->PinName.ToString() + TEXT("\n");
        return Result;
    }
    Root->NodePosX = 150;
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    return TEXT("OK: right finger pose layered over input animation");
#else
    return TEXT("ERROR: editor only");
#endif
}

bool UWarbandsEditorBridge::SetSwordGripPose(UObject* BlueprintAsset, UObject* PoseAsset)
{
#if WITH_EDITOR
    UAnimBlueprint* BP = Cast<UAnimBlueprint>(BlueprintAsset);
    UAnimSequence* Sequence = Cast<UAnimSequence>(PoseAsset);
    if (!BP || !Sequence) return false;
    TArray<UEdGraph*> Graphs;
    BP->GetAllGraphs(Graphs);
    for (UEdGraph* Graph : Graphs)
        for (UEdGraphNode* Node : Graph->Nodes)
            if (auto* Evaluator = Cast<UAnimGraphNode_SequenceEvaluator>(Node))
            {
                BP->Modify();
                Evaluator->Modify();
                Evaluator->Node.SetSequence(Sequence);
                FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
                return true;
            }
#endif
    return false;
}
