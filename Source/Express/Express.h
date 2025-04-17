// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTrain, Log, All);

#define PRINTFATALLOG(fmt, ...) UE_LOG(LogTrain, Fatal, TEXT("ERROR: [%s] (%d): %s"), *FString(__FUNCTION__), __LINE__, *FString::Printf(fmt, ##__VA_ARGS__))
