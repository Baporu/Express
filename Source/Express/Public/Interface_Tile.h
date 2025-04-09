// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interface_Tile.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInterface_Tile : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class EXPRESS_API IInterface_Tile
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void PressKey() {};
	virtual void DestroyTile() {};
};
