#pragma once
#include "CoreMinimal.h"
#include "KismetNodes/SGraphNodeK2Base.h"

class UBlueprintSortArrayNode;
class SVerticalBox;

class SAddButtonGraphNode : public SGraphNodeK2Base
{
public:

	SLATE_BEGIN_ARGS(SAddButtonGraphNode) {}
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs, UBlueprintSortArrayNode* InNode);

protected:
	virtual void CreateInputSideAddButton(TSharedPtr<SVerticalBox> InputBox);
	virtual FReply OnAddPin();
};

