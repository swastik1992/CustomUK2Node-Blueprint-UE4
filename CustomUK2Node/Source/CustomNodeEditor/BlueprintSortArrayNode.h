#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "BlueprintSortArrayNode.generated.h"

UCLASS()
class CUSTOMNODEEDITOR_API UBlueprintSortArrayNode : public UK2Node
{
	GENERATED_BODY()

public:

	//UEdGraphNode interface
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("A template funtion to sort array.")); }
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return FText::FromString(TEXT("SORTARRAY")); }
	virtual void AllocateDefaultPins() override;
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	//End of UEdGraphNode interface

	//UK2Node interface.
	virtual FText GetMenuCategory() const { return FText::FromString(TEXT("Array")); }
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	//End of UK2Node interface.

	virtual void AddInputPinToNode();
	virtual void RemoveInputPinFromNode(UEdGraphPin* Pin);

protected:

	UPROPERTY()
	TArray<FName> ArgStringInputPinNames;

	UPROPERTY()
	TArray<FName> ArgBooleanInputPinNames;
};

