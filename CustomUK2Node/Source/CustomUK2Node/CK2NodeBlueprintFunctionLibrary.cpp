// Fill out your copyright notice in the Description page of Project Settings.


#include "CK2NodeBlueprintFunctionLibrary.h"

void UCK2NodeBlueprintFunctionLibrary::SortArray(const TArray< FArgsInput>& InArgs)
{
	for (const FArgsInput& Arg : InArgs)
	{
		UE_LOG(LogTemp, Log, TEXT("Input Field is: %s"), *(Arg.ArgStringInputPinName.ToString()));
		UE_LOG(LogTemp, Log, TEXT("Input Order is: %s"), *(Arg.ArgStringInputPinName.ToString()));
	}
}