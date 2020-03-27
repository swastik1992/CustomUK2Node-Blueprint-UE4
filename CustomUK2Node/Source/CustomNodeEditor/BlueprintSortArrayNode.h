#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "BlueprintSortArrayNode.generated.h"

USTRUCT(BlueprintType)
struct FArgsInput
{
	GENERATED_BODY()

	FArgsInput(){}
	
	FArgsInput(const FName& StringName, const FName& BooleanName)
		: ArgStringInputPinName(StringName), ArgBooleanInputPinName(BooleanName)
	{}

	UPROPERTY()
	FName ArgStringInputPinName;

	UPROPERTY()
	FName ArgBooleanInputPinName;
};

USTRUCT()
struct FArgsOutput
{
	GENERATED_BODY()

	FArgsOutput() {}

	FArgsOutput(const FName& WildCardName, const FName& BooleanName)
		: ArgWildCardOutputPinName(WildCardName), ArgBooleanOutputPinName(BooleanName)
	{}

	UPROPERTY()
	FName ArgWildCardOutputPinName;

	UPROPERTY()
	FName ArgBooleanOutputPinName;
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

	virtual void AddInputPinToNode();
	virtual void RemoveInputPinFromNode(UEdGraphPin* Pin);

protected:

	UPROPERTY()
	TArray<FArgsInput> ArgsInputArray;
	
	UPROPERTY()
	TArray<FArgsOutput> ArgsOutputArray;

	UPROPERTY()
	int32 LastIndex;
};

