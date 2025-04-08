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
        UE_LOG(LogTemp, Warning, TEXT("TileHeights is empty!"));
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
                    UE_LOG(LogTemp, Warning, TEXT("TileActorClass is not set!"));
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
    if (!HeightDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("HeightData Null"));
        NumRows = 5; NumCols = 5;  // 기본값
        TileHeights.SetNum(NumRows);
        for (int32 Row = 0; Row < NumRows; Row++)
        {
            TileHeights[Row].SetNum(NumCols);
            for (int32 Col = 0; Col < NumCols; Col++)
            {
                TileHeights[Row][Col] = 0;  // 기본 높이 0
            }
        }
        return;
    }

    TArray<FName> RowNames = HeightDataTable->GetRowNames();  // CSV 행 이름
    NumRows = RowNames.Num();  // 높이는 ㅎ
    TileHeights.Empty();  // 초기화
    TileHeights.SetNum(NumRows);  // 행 수 설정

    FTileHeightData* FirstRow = HeightDataTable->FindRow<FTileHeightData>(RowNames[0], "");
    if (FirstRow)
    {
        TArray<FString> ColData;
        FirstRow->Height.ParseIntoArray(ColData, TEXT(","), true);  // 첫 행 열 수 계산
        NumCols = ColData.Num();  // 열 수 설정
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("FirstRow not found!"));
        NumCols = 5;  // 기본값
    }

    for (const FName& RowName : RowNames)
    {
        FTileHeightData* Row = HeightDataTable->FindRow<FTileHeightData>(RowName, "");
        if (Row)
        {
            TArray<FString> ColData;
            Row->Height.ParseIntoArray(ColData, TEXT(","), true);  // 높이값 분리
            int32 RowIdx = FCString::Atoi(*RowName.ToString());  // 행 인덱스 (0~4)
            if (TileHeights.IsValidIndex(RowIdx))
            {
                TileHeights[RowIdx].SetNum(NumCols);  // 열 수 설정
                for (int32 Col = 0; Col < ColData.Num() && Col < NumCols; Col++)
                {
                    TileHeights[RowIdx][Col] = FCString::Atoi(*ColData[Col]);  // 높이값 저장
                }
            }
        }
    }
}

