// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface_Tile.h"
#include "Wood_Tile.generated.h"

UCLASS()
class EXPRESS_API AWood_Tile : public AActor, public IInterface_Tile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWood_Tile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
