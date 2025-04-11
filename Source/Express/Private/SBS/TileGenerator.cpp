// Fill out your copyright notice in the Description page of Project Settings.


#include "SBS/TileGenerator.h"

// Sets default values
ATileGenerator::ATileGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATileGenerator::BeginPlay()
{
	Super::BeginPlay();
    GenerateMap();

    if (TileHeights.Num() == 0 || TileHeights[0].Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("TileHeights empty"));
        return;
    }
	if (TileTypes.Num() == 0 || TileTypes[0].Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("TileTypes empty"));
		return;
	}
    for (int32 Row = 0; Row < NumRows; Row++)
    {
        for (int32 Col = 0; Col < NumCols; Col++)
        {
            int32 MaxHeight = TileHeights[Row][Col];
            for (int32 Z = 0; Z <= MaxHeight; Z++)
            {
                FVector Location(Row * 100.f, Col * 100.f, Z * 100.f);
                if (TileActorClass)
                {
                    GetWorld()->SpawnActor<ATile>(TileActorClass, Location, FRotator::ZeroRotator);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("TileActor null"));
                }
            }
        }
    }
}

// Called every frame
void ATileGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATileGenerator::GenerateMap()
{
    if (!HeightDataTable || !TypeDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("HeightDataTable or TypeDataTable is null, using default"));
        NumRows = 5; NumCols = 5;
        TileHeights.SetNum(NumRows);
        TileTypes.SetNum(NumRows);
        for (int32 Row = 0; Row < NumRows; Row++)
        {
            TileHeights[Row].SetNum(NumCols);
            TileTypes[Row].SetNum(NumCols);
            for (int32 Col = 0; Col < NumCols; Col++)
            {
                TileHeights[Row][Col] = 0;
                TileTypes[Row][Col] = ETileType::Ground;
            }
        }
        return;
    }

    // ³ôÀÌ µ¥ÀÌÅÍ ÆÄ½Ì
    TArray<FName> HeightRowNames = HeightDataTable->GetRowNames();
    NumRows = HeightRowNames.Num();
    TileHeights.SetNum(NumRows);

    for (int32 Row = 0; Row < NumRows; Row++)
    {
        FTileHeightData* RowData = HeightDataTable->FindRow<FTileHeightData>(HeightRowNames[Row], "");
        if (RowData)
        {
            TArray<FString> ColData;
            RowData->Height.ParseIntoArray(ColData, TEXT(","), true); // ½°Ç¥·Î ºÐ¸®
            if (Row == 0) NumCols = ColData.Num();
            if (ColData.Num() != NumCols)
            {
                UE_LOG(LogTemp, Warning, TEXT("Height row %d has inconsistent column count!"), Row);
                continue;
            }
            TileHeights[Row].SetNum(NumCols);
            for (int32 Col = 0; Col < NumCols; Col++)
            {
                TileHeights[Row][Col] = FCString::Atoi(*ColData[Col]);
            }
        }
    }

    // Å¸ÀÔ µ¥ÀÌÅÍ ÆÄ½Ì
    TArray<FName> TypeRowNames = TypeDataTable->GetRowNames();
    if (TypeRowNames.Num() != NumRows)
    {
        UE_LOG(LogTemp, Warning, TEXT("TypeDataTable row count (%d) doesn't match HeightDataTable (%d)!"), TypeRowNames.Num(), NumRows);
        return;
    }
    TileTypes.SetNum(NumRows);

    for (int32 Row = 0; Row < NumRows; Row++)
    {
        FTileTypeData* RowData = TypeDataTable->FindRow<FTileTypeData>(TypeRowNames[Row], "");
        if (RowData)
        {
            TArray<FString> ColData;
            RowData->Type.ParseIntoArray(ColData, TEXT(","), true); // ½°Ç¥·Î ºÐ¸®
            if (ColData.Num() != NumCols)
            {
                UE_LOG(LogTemp, Warning, TEXT("Type row %d has inconsistent column count!"), Row);
                continue;
            }
            TileTypes[Row].SetNum(NumCols);
            for (int32 Col = 0; Col < NumCols; Col++)
            {
                if (ColData[Col] == TEXT("W")) TileTypes[Row][Col] = ETileType::Wood;
                else if (ColData[Col] == TEXT("G")) TileTypes[Row][Col] = ETileType::Ground;
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Unknown TileType '%s' at Row %d, Col %d, setting to Ground"), *ColData[Col], Row, Col);
                    TileTypes[Row][Col] = ETileType::Ground;
                }
            }
        }
    }
}

