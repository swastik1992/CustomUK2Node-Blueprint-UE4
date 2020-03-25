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

void UBlueprintSortArrayNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

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

	BreakAllNodeLinks();
}

TSharedPtr<SGraphNode> UBlueprintSortArrayNode::CreateVisualWidget()
{
	return SNew(SAddButtonGraphNode, this);
}

void UBlueprintSortArrayNode::AddInputPinToNode()
{
	TMap<FString, FStringFormatArg> ArgsString = { {TEXT("Count"),ArgStringInputPinNames.Num()} };
	FName NewStringPinName(*FString::Format(TEXT("Field {Count}"), ArgsString));
	UE_LOG(LogTemp, Log, TEXT("AddInputPinToNode(): New Pin's Name is %s"), *NewStringPinName.ToString());
	ArgStringInputPinNames.Add(NewStringPinName);

	TMap<FString, FStringFormatArg> ArgsBoolean = { {TEXT("Count"),ArgBooleanInputPinNames.Num()} };
	FName NewBooleanPinName(*FString::Format(TEXT("Ascending Order {Count}"), ArgsBoolean));
	UE_LOG(LogTemp, Log, TEXT("AddInputPinToNode(): New Pin's Name is %s"), *NewBooleanPinName.ToString());
	ArgBooleanInputPinNames.Add(NewBooleanPinName);
	
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_String, NewStringPinName);
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, NewBooleanPinName);

}

void UBlueprintSortArrayNode::RemoveInputPinFromNode(UEdGraphPin* Pin)
{

}