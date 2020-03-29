#include "BlueprintSortArrayNode.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "EdGraphSchema_K2.h"	
#include "K2Node_CallFunction.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraph.h"
#include "KismetCompiler.h"
#include "CK2NodeBlueprintFunctionLibrary.h"
#include "AddButtonGraphNode.h"
#include "ToolMenus.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "K2Node_MakeArray.h"

void UBlueprintSortArrayNode::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* NodeClass = GetClass();
	if (!ActionRegistrar.IsOpenForRegistration(NodeClass))
	{
		return;
	}

	UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(NodeClass);
	check(NodeSpawner);
	ActionRegistrar.AddBlueprintAction(NodeClass, NodeSpawner);
}

void UBlueprintSortArrayNode::GetNodeContextMenuActions(UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);

	if (Context->bIsDebugging)
	{
		return;
	}

	FName SectionName = FName(*FString("UBlueprintSortArrayNode"));
	FToolMenuSection& Section = Menu->FindOrAddSection(SectionName);

	if (Context->Pin == nullptr)
	{
		return;
	}

	if (Context->Pin->Direction == EGPD_Input && Context->Pin->ParentPin == nullptr)
	{
		Section.AddMenuEntry(FName(*FString("UBlueprintSortArrayNode")), FText::FromString(TEXT("Remove entry")),
			FText::FromString(TEXT("Remove entry")),FSlateIcon(),
			FUIAction(FExecuteAction::CreateUObject(this, &UBlueprintSortArrayNode::RemoveDynamicPinFromNode,
			const_cast<UEdGraphPin*>(Context->Pin))));
	}
}


void UBlueprintSortArrayNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	for (const FArgs& Arg : ArgsArrayNames)
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, Arg.InputArgPinName);
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, Arg.OutputArgPinName);
	}

	Super::AllocateDefaultPins();
}

void UBlueprintSortArrayNode::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	UEdGraphPin* ExecPin = GetExecPin();
	if (!ExecPin)
	{
		return;
	}

	UEdGraphPin* ThenPin = FindPin(UEdGraphSchema_K2::PN_Then);
	if (!ThenPin || ThenPin->Direction != EGPD_Output)
	{
		return;
	}

	UK2Node_CallFunction* CallFunctionNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	if (!CallFunctionNode)
	{
		return;
	}

	FName FunctionName = GET_FUNCTION_NAME_CHECKED(UCK2NodeBlueprintFunctionLibrary, SortArray);
	CallFunctionNode->FunctionReference.SetExternalMember(FunctionName, UCK2NodeBlueprintFunctionLibrary::StaticClass());
	CallFunctionNode->AllocateDefaultPins();

	CompilerContext.MovePinLinksToIntermediate(*ExecPin, *(CallFunctionNode->GetExecPin()));
	CompilerContext.MovePinLinksToIntermediate(*ThenPin, *(CallFunctionNode->GetThenPin()));
	
	
	UK2Node_MakeArray* MakeArrayNode = CompilerContext.SpawnIntermediateNode<UK2Node_MakeArray>(this, SourceGraph);
	if (!MakeArrayNode)
	{
		return;
	}
	MakeArrayNode->AllocateDefaultPins();

	UEdGraphPin* ArrayNodeOutPin = MakeArrayNode->GetOutputPin();
	UEdGraphPin* ArgCallFunctionNodePin = CallFunctionNode->FindPinChecked(TEXT("InArgs"));
	ArrayNodeOutPin->MakeLinkTo(ArgCallFunctionNodePin);
	
	MakeArrayNode->PinConnectionListChanged(ArrayNodeOutPin);

	int32 Index = 0;
	for (int32 i = 0; i < ArgsArrayNames.Num(); ++i)
	{
		if (i > 0) //UK2Node_MakeArray would have 1 node by default.
		{
			MakeArrayNode->AddInputPin();
		}
		
		const FString MakeArrayPinName = FString::Printf(TEXT("[%d]"), i);
		UEdGraphPin* MakeArrayInputPin = MakeArrayNode->FindPinChecked(MakeArrayPinName);
		
		UEdGraphPin* InputPin = FindPinChecked(ArgsArrayNames[i].InputArgPinName, EGPD_Input);
		
		CompilerContext.MovePinLinksToIntermediate(*InputPin, *MakeArrayInputPin);
	}

	BreakAllNodeLinks();
}

TSharedPtr<SGraphNode> UBlueprintSortArrayNode::CreateVisualWidget()
{
	return SNew(SAddButtonGraphNode, this);
}

void UBlueprintSortArrayNode::AddDynamicPinToNode()
{
	if (ArgsArrayNames.Num() <= 0)
	{
		LastIndex = 0;
	}

	TMap<FString, FStringFormatArg> Args = { {TEXT("Count"),LastIndex} };
	FName NewInputPinName(*FString::Format(TEXT("Sort Data: {Count}"), Args));
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FArgsInput::StaticStruct(), NewInputPinName);

	UEdGraphNode::FCreatePinParams OutputPinParams;
	OutputPinParams.bIsReference = true;
	FName NewOutputPinName(*FString::Format(TEXT("Sort Result: {Count}"), Args));
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, FArgsOutput::StaticStruct(), NewOutputPinName, OutputPinParams);

	ArgsArrayNames.Add(FArgs(NewInputPinName, NewOutputPinName));

	LastIndex++;
}

void UBlueprintSortArrayNode::RemoveDynamicPinFromNode(UEdGraphPin* Pin)
{
	this->Modify();

	int32 Index = -1;
	for (int32 i = 0; i < ArgsArrayNames.Num(); ++i)
	{
		if ((ArgsArrayNames[i].InputArgPinName == Pin->GetFName()) ||
			(ArgsArrayNames[i].OutputArgPinName == Pin->GetFName()))
		{
			Index = i;
			break;
		}
	}

	if (Index == -1)
	{
		return;
	}

	UEdGraphPin* InputPin = this->FindPin(ArgsArrayNames[Index].InputArgPinName, EGPD_Input);
	UEdGraphPin* OutputPin = this->FindPin(ArgsArrayNames[Index].OutputArgPinName, EGPD_Output);

	if (InputPin == nullptr || OutputPin == nullptr)
	{
		return;
	}

	this->RemovePin(InputPin);
	this->RemovePin(OutputPin);

	ArgsArrayNames.RemoveAt(Index);

	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}


