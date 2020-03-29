#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "BlueprintSortArrayNode.generated.h"

USTRUCT()
struct FArgs
{
	GENERATED_BODY()

	FArgs() {}

	FArgs(const FName& InputArgName, const FName& OutputArgName)
		: InputArgPinName(InputArgName), OutputArgPinName(OutputArgName)
	{}

	UPROPERTY()
	FName InputArgPinName;

	UPROPERTY()
	FName OutputArgPinName;
};

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
	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	//End of UEdGraphNode interface

	//UK2Node interface.
	virtual FText GetMenuCategory() const { return FText::FromString(TEXT("Array")); }
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	//End of UK2Node interface.

	virtual void AddDynamicPinToNode();
	virtual void RemoveDynamicPinFromNode(UEdGraphPin* Pin);

protected:

	UPROPERTY()
	TArray<FArgs> ArgsArrayNames;

	UPROPERTY()
	int32 LastIndex;
};

