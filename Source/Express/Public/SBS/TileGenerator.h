// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "TileGenerator.generated.h"

UCLASS()
class EXPRESS_API ATileGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, Category = "Data")
    UDataTable* HeightDataTable;  // CSV 데이터 테이블

	//UPROPERTY(EditAnywhere, Category = "Data")
	//UDataTable* TypeDataTable;  // CSV 데이터 테이블

	UPROPERTY(EditAnywhere, Category = "Tile")
    TSubclassOf<ATile> TileActorClass;  // 스폰할 Tile

	TArray<TArray<int32>> TileHeights;

	int NumRows; //가로
	int NumCols; //세로

	void GenerateMap();
};
