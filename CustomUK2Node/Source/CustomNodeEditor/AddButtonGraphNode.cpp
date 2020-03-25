#include "AddButtonGraphNode.h"
#include "BlueprintSortArrayNode.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "GraphEditorSettings.h"
#include "SGraphPin.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "BlueprintSortArrayNode.h"

void SAddButtonGraphNode::Construct(const FArguments& InArgs, UBlueprintSortArrayNode* InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

void SAddButtonGraphNode::CreateInputSideAddButton(TSharedPtr<SVerticalBox> InputBox)
{
	TSharedRef<SWidget> PinButton = AddPinButtonContent(FText::FromString(TEXT("Add Field and Order")), FText::FromString(TEXT("Add Field and Order")));
	FMargin InputPinPadding = Settings->GetInputPinPadding();
	InputPinPadding.Top += 6.0f;

	InputBox->AddSlot().AutoHeight().VAlign(VAlign_Center).Padding(InputPinPadding)
	[PinButton];
}

FReply SAddButtonGraphNode::OnAddPin()
{
	UBlueprintSortArrayNode* SortArrayNode = CastChecked<UBlueprintSortArrayNode>(this->GraphNode);
	if (SortArrayNode == nullptr)
	{
		return FReply::Unhandled();
	}

	if (!SortArrayNode->CanModify())
	{
		return FReply::Unhandled();
	}
	
	SortArrayNode->Modify();
	SortArrayNode->AddInputPinToNode();
	FBlueprintEditorUtils::MarkBlueprintAsModified(SortArrayNode->GetBlueprint());

	UpdateGraphNode();
	GraphNode->GetGraph()->NotifyGraphChanged();

	return FReply::Handled();
}