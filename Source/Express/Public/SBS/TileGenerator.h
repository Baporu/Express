// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "Engine/DataTable.h"
#include "TileGenerator.generated.h"

USTRUCT(BlueprintType)
struct FTileHeightData : public FTableRowBase
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Height; // "5,5,5,5,5"
};
USTRUCT(BlueprintType)
struct FTileTypeData : public FTableRowBase
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Type; // "W,W,W,W,W" 
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
    UDataTable* HeightDataTable; // CSV ������ ���̺� (����)

    UPROPERTY(EditAnywhere, Category = "Data")
    UDataTable* TypeDataTable;   // CSV ������ ���̺� (Ÿ��)

    UPROPERTY(VisibleAnywhere, Category = "Tile")
    int NumRows = 5;

    UPROPERTY(VisibleAnywhere, Category = "Tile")
    int NumCols = 5;
    bool bHasGenerated = false;


    TArray<TArray<int>> TileHeights;
    TArray<TArray<ETileType>> TileTypes;

    UPROPERTY(Replicated, EditAnywhere, Category = "Tile")
    class AGridManager* GridManager;

	void GenerateMap();

private:
    UPROPERTY(EditDefaultsOnly, Category = "Train")
	TSubclassOf<class ATrainEngine> TrainFactory;


	void SetTrain(ATile* Tile, int32 TileRow, int32 TileColumn);

//��Ʈ��ũ
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
