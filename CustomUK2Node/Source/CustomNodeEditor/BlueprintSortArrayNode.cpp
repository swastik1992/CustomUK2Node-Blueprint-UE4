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
			FUIAction(FExecuteAction::CreateUObject(this, &UBlueprintSortArrayNode::RemoveInputPinFromNode, 
			const_cast<UEdGraphPin*>(Context->Pin))));
	}
}


void UBlueprintSortArrayNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	for (const FArgsInput& Arg : ArgsInputArray)
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, Arg.ArgStringInputPinName);
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, Arg.ArgBooleanInputPinName);
	}

	for (const FArgsOutput& Arg : ArgsOutputArray)
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, Arg.ArgWildCardOutputPinName);
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, Arg.ArgBooleanOutputPinName);
	}


	Super::AllocateDefaultPins();
}

void UBlueprintSortArrayNode::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
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
	UEdGraphPin* ArgCallFunctionNodePin = CallFunctionNode->FindPinChecked(TEXT("InArg"));
	ArrayNodeOutPin->MakeLinkTo(ArgCallFunctionNodePin);
	
	MakeArrayNode->PinConnectionListChanged(ArrayNodeOutPin);

	int32 Index = 0;
	for (int32 i = 0; i < ArgsInputArray.Num(); ++i)
	{
		if (i > 0)
		{
			MakeArrayNode->AddInputPin();
			MakeArrayNode->AddInputPin();
		}
		else //UK2Node_MakeArray would have 1 node by default.
		{
			MakeArrayNode->AddInputPin();
		}

		const FString MakeArrayStrPinName = FString::Printf(TEXT("[%d]"), Index);
		UEdGraphPin* MakeArrayStrInputPin = MakeArrayNode->FindPinChecked(MakeArrayStrPinName);
		Index++;

		const FString MakeArrayBoolPinName = FString::Printf(TEXT("[%d]"), Index);
		UEdGraphPin* MakeArrayBoolInputPin = MakeArrayNode->FindPinChecked(MakeArrayBoolPinName);
		Index++;

		UEdGraphPin* StringInputPin = FindPinChecked( ArgsInputArray[i].ArgStringInputPinName, EGPD_Input);
		UEdGraphPin* BoolInputPin = FindPinChecked( ArgsInputArray[i].ArgBooleanInputPinName, EGPD_Input);
		
		CompilerContext.MovePinLinksToIntermediate(*MakeArrayStrInputPin, *StringInputPin);
		CompilerContext.MovePinLinksToIntermediate(*MakeArrayBoolInputPin, *BoolInputPin);
	}

	BreakAllNodeLinks();
}

TSharedPtr<SGraphNode> UBlueprintSortArrayNode::CreateVisualWidget()
{
	return SNew(SAddButtonGraphNode, this);
}

void UBlueprintSortArrayNode::AddInputPinToNode()
{
	if (ArgsInputArray.Num() <= 0 && ArgsOutputArray.Num() <= 0)
	{
		LastIndex = 0;
	}
	
	TMap<FString, FStringFormatArg> Args = { {TEXT("Count"),LastIndex} };
	
	//input pins.
	FName NewStringPinName(*FString::Format(TEXT("Field {Count}"), Args));
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, NewStringPinName);

	FName NewBooleanPinName(*FString::Format(TEXT("Ascending Order {Count}"), Args));
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, NewBooleanPinName);
	
	ArgsInputArray.Add(FArgsInput(NewStringPinName, NewBooleanPinName));
	//end of input pins

	//output pins
	FName NewWildCardPinName(*FString::Format(TEXT("Array {Count}"), Args));
	UEdGraphNode::FCreatePinParams OutputPinParams;
	OutputPinParams.bIsReference = true;
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, NewWildCardPinName, OutputPinParams);

	FName NewBooleanOutputPinName(*FString::Format(TEXT("Result {Count}"), Args));
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, NewBooleanOutputPinName, OutputPinParams);
	
	ArgsOutputArray.Add(FArgsOutput(NewWildCardPinName, NewBooleanOutputPinName));
	//end of output pins.

	LastIndex++;
}

void UBlueprintSortArrayNode::RemoveInputPinFromNode(UEdGraphPin* Pin)
{
	this->Modify();

	int32 Index = -1;
	for (int32 i = 0; i < ArgsInputArray.Num(); ++i)
	{
		if ((ArgsInputArray[i].ArgStringInputPinName == Pin->GetFName()) ||
			(ArgsInputArray[i].ArgBooleanInputPinName == Pin->GetFName()))
		{
			Index = i;
			break;
		}
	}

	if (Index == -1 || !ArgsOutputArray.IsValidIndex(Index))
	{
		return;
	}

	UEdGraphPin* StringInputPin = this->FindPin(ArgsInputArray[Index].ArgStringInputPinName, EGPD_Input);
	UEdGraphPin* BoolInputPin = this->FindPin(ArgsInputArray[Index].ArgBooleanInputPinName, EGPD_Input);
	UEdGraphPin* WildCardOutputPin = this->FindPin(ArgsOutputArray[Index].ArgWildCardOutputPinName, EGPD_Output);
	UEdGraphPin* BoolOutputPin = this->FindPin(ArgsOutputArray[Index].ArgBooleanOutputPinName, EGPD_Output);
	
	if (StringInputPin == nullptr || BoolInputPin == nullptr
		|| WildCardOutputPin == nullptr || BoolOutputPin == nullptr)
	{
		return;
	}

	this->RemovePin(StringInputPin);
	this->RemovePin(BoolInputPin);
	this->RemovePin(WildCardOutputPin);
	this->RemovePin(BoolOutputPin);

	ArgsInputArray.RemoveAt(Index);
	ArgsOutputArray.RemoveAt(Index);
	
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());

}

