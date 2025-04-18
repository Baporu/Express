// Fill out your copyright notice in the Description page of Project Settings.


#include "SBS/TileGenerator.h"
#include "Express/Express.h"
#include "SHS/GridManager.h"

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

  
    if (bHasGenerated)
    {
        return;
    }

    bHasGenerated = true;
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
    for (int Row = 0; Row < NumRows; Row++)
    {
        TArray<ATile*> ColTiles;

        for (int Col = 0; Col < NumCols; Col++)
        {
            int MaxHeight = TileHeights[Row][Col]; 
            //����
            for (int Z = 0; Z <= MaxHeight; Z++)
            {
                FVector Location((NumRows - 1 -Row) * 100.f, Col * 100.f, Z * 100.f);
                if (TileActorClass)
                {
                    ATile* NewTile = GetWorld()->SpawnActor<ATile>(TileActorClass, Location, FRotator::ZeroRotator);
                    if (Z == 0 && (TileTypes[Row][Col] != ETileType::Water))
                    {
                        NewTile->CreateTile(ETileType::Ground);

                        NewTile->gridRow = Row;
                        NewTile->gridColumn = Col;
                        NewTile->GridManager = GridManager;

                        ColTiles.Add(NewTile);
                    }
                    else
                    {
                        NewTile->CreateTile(TileTypes[Row][Col]);
                        //NewTile->TileType = TileTypes[Row][Col];
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("TileActor null"));
                }
            }
        }

        GridManager->Grid.Add(ColTiles);
    }

    for (int i = 0; i < GridManager->Grid.Num(); i++) {
        for (int j = 0; j < GridManager->Grid[i].Num(); j++)
            UE_LOG(LogTrain, Log, TEXT("Row: %d, Col: %d, Name: %s"), i, j, *GridManager->Grid[i][j]->GetActorNameOrLabel());
    }
}

// Called every frame
void ATileGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATileGenerator::GenerateMap()
{
    //������ �ȵ���
    if (!HeightDataTable || !TypeDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("Data Null"));
        return;
    }

    // ���� ������ �ޱ�
    TArray<FName> HeightRowNames = HeightDataTable->GetRowNames(); //�� �̸� ��ȸ 1~10
    NumRows = HeightRowNames.Num(); //10
    TileHeights.SetNum(NumRows); //Tileheight�� 10���� �� �迭�� ����

    for (int Row = 0; Row < NumRows; Row++)
    {
        FTileHeightData* RowData = HeightDataTable->FindRow<FTileHeightData>(HeightRowNames[Row], ""); //Rowdata�� Heightdatatable�� 0������ �ʱ�ȭ 1,2,3,...
        TArray<FString> ColData; //�� ����
        RowData->Height.ParseIntoArray(ColData, TEXT(","), true); // Coldata = { "0", "2", "3", "2", "1", "1", "1", "2", "2", "0" }. 

        if (Row == 0) 
        {
            NumCols = ColData.Num();
        }
        TileHeights[Row].SetNum(NumCols);
        for (int Col = 0; Col < NumCols; Col++)
        {
            TileHeights[Row][Col] = FCString::Atoi(*ColData[Col]);
        }
        
    }

    // Ÿ�� ������ �ޱ�
    TArray<FName> TypeRowNames = TypeDataTable->GetRowNames(); //��� �� �̸�
    TileTypes.SetNum(NumRows);

    for (int Row = 0; Row < NumRows; Row++) //��ݺ�
    {
        FTileTypeData* RowData = TypeDataTable->FindRow<FTileTypeData>(TypeRowNames[Row], ""); //��ã��
		TArray<FString> ColData;
		RowData->Type.ParseIntoArray(ColData, TEXT(","), true); //Ÿ�� ������ �Ľ�
		TileTypes[Row].SetNum(NumCols);
		for (int Col = 0; Col < NumCols; Col++) //���ݺ�
		{
            if (ColData[Col].Contains("W")) TileTypes[Row][Col] = ETileType::Wood; //W�� wood
            else if (ColData[Col].Contains("G")) TileTypes[Row][Col] = ETileType::Ground;
            else if (ColData[Col].Contains("S")) TileTypes[Row][Col] = ETileType::Stone;
            else if (ColData[Col].Contains("R")) TileTypes[Row][Col] = ETileType::Rock;
            else if (ColData[Col].Contains("O")) TileTypes[Row][Col] = ETileType::Water;
            else if (ColData[Col].Contains("T")) TileTypes[Row][Col] = ETileType::Rail;
            else if (ColData[Col].Contains("A")) TileTypes[Row][Col] = ETileType::Station_A;
            else if (ColData[Col].Contains("Z")) TileTypes[Row][Col] = ETileType::Station_Z;
			else
		    {
				UE_LOG(LogTemp, Warning, TEXT("TYPE Wrong"));
			}
		}
        
    }
}

