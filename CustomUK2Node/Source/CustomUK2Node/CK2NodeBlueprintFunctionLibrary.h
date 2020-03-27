// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include  "CustomNodeEditor/BlueprintSortArrayNode.h"//BlueprintSortArrayNode.h"
#include "CK2NodeBlueprintFunctionLibrary.generated.h"
/**
 * 
 */
UCLASS()
class CUSTOMUK2NODE_API UCK2NodeBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "True"))
	static void SortArray(const TArray< FArgsInput>& InArgs);

	
};
