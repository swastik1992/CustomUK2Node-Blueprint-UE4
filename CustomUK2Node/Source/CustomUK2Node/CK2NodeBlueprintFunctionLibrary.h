// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CK2NodeBlueprintFunctionLibrary.generated.h"
/**
 * 
 */

USTRUCT(BlueprintType)
struct FArgsInput
{
	GENERATED_USTRUCT_BODY()

	FArgsInput() {}

	FArgsInput(const FString& Name, const bool Order)
		: FieldName(Name), bAscendingOrder(Order)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sort")
	FString FieldName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sort")
	bool bAscendingOrder;
};

USTRUCT(BlueprintType)
struct FArgsOutput
{
	GENERATED_USTRUCT_BODY()

	FArgsOutput() {}

	FArgsOutput(const TArray<AActor*>& InWildCardArray, const bool InResult)
		: WildCardArray(InWildCardArray), bResult(InResult)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sort")
	TArray<AActor*> WildCardArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sort")
	bool bResult;
};

UCLASS()
class CUSTOMUK2NODE_API UCK2NodeBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "True"))
	static void SortArray(const TArray< FArgsInput>& InArgs);

	
};
