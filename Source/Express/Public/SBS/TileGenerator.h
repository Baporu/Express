// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "TileGenerator.generated.h"

USTRUCT(BlueprintType)
struct FTileHeightData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Height; // "5,5,5,5,5" 형식
};
USTRUCT(BlueprintType)
struct FTileTypeData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Type; // "W,W,W,W,W" 형식
};

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
    UPROPERTY(EditAnywhere, Category = "Tile")
    TSubclassOf<ATile> TileActorClass;

    UPROPERTY(EditAnywhere, Category = "Data")
    UDataTable* HeightDataTable; // CSV 데이터 테이블 (높이)

    UPROPERTY(EditAnywhere, Category = "Data")
    UDataTable* TypeDataTable;   // CSV 데이터 테이블 (타입)

    UPROPERTY(VisibleAnywhere, Category = "Tile")
    int32 NumRows = 5;

    UPROPERTY(VisibleAnywhere, Category = "Tile")
    int32 NumCols = 5;

    UPROPERTY(VisibleAnywhere, Category = "Tile")
    TArray<TArray<int32>> TileHeights;

    UPROPERTY(VisibleAnywhere, Category = "Tile")
    TArray<TArray<ETileType>> TileTypes;

	void GenerateMap();
};
