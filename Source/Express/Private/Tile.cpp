// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"
#include "Tile_FSM.h"
#include "Exp_GameMode.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Chaos/Vector.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tile Mesh"));
	RootComponent = TileMesh;
	TileFSM = CreateDefaultSubobject<UTile_FSM>(TEXT("Tile FSM"));
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

	GenerateMap(); // �� ����

	//Height = HeightDataTable->GetRowNames().Num(); // �హ��
	//Width = HeightDataTable->GetRowNames().Num(); // ������

	for (int32 Row = 0; Row < Height; Row++)
	{
		for (int32 Col = 0; Col < Width; Col++)
		{
			FVector Location(Row * 100.f, Col * 100.f, TileHeights[Row][Col] * 100.f);  // X, Y, Z ��ġ ����
			GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), Location, FRotator::ZeroRotator);  // �⺻ ���� ����
			UE_LOG(LogTemp, Warning, TEXT("%d  %d", Row, Col));
		}
	}

	//GenerateTiles(HeightData);
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FTileHeightData ATile::ReadHeightDataFromCSV(const FString& FilePath)
{
	FTileHeightData HeightData;
	TArray<FString> FileLines;

	if (FFileHelper::LoadFileToStringArray(FileLines, *FilePath))
	{
		for (const FString& Line : FileLines)
		{
			TArray<FString> Values;
			Line.ParseIntoArray(Values, TEXT(","), true);

			TArray<float> Row;
			for (const FString& Value : Values)
			{
				Row.Add(FCString::Atof(*Value));
			}
		}
	}
return HeightData;
}

void ATile::GenerateMap()
{
	if (!HeightDataTable) return;  // ������ ���̺� ������ ����

	TArray<FName> RowNames = HeightDataTable->GetRowNames();  // CSV�� ��� �� �̸�
	Height = RowNames.Num();  // �� ���� CSV �� ���� ����
	TileHeights.Empty();  // ���� ������ �ʱ�ȭ
	TileHeights.SetNum(Height);  // �� �� ����

	// ù ��° �࿡�� �� �� ���
	FTileHeightData* FirstRow = HeightDataTable->FindRow<FTileHeightData>(RowNames[0], "");
	if (FirstRow)
	{
		TArray<FString> ColData;
		FirstRow->Height.ParseIntoArray(ColData, TEXT(","), true);  // ù ���� ���̰� �и�
		Width = ColData.Num();  // �� ���� CSV �� ���� ����
	}

	// �� �࿡ ������ ä���
	for (const FName& RowName : RowNames)
	{
		FTileHeightData* Row = HeightDataTable->FindRow<FTileHeightData>(RowName, "");  // �� ������ ��ȸ
		if (Row)
		{
			TArray<FString> ColData;
			Row->Height.ParseIntoArray(ColData, TEXT(","), true);  // ���̰� ��ǥ�� �и�
			int32 RowIdx = FCString::Atoi(*RowName.ToString());  // �� �̸� ������ ��ȯ (��: "0" -> 0)
			if (TileHeights.IsValidIndex(RowIdx))
			{
				TileHeights[RowIdx].SetNum(Width);  // �� �� ����
				for (int32 Col = 0; Col < ColData.Num() && Col < Width; Col++)
				{
					TileHeights[RowIdx][Col] = FCString::Atoi(*ColData[Col]);  // ���̰� ����
				}
			}
		}
	}

}

