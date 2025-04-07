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

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder(TEXT("/Game/SBS/Source/TileHeightData.TileHeightData"));
	if (DataTableFinder.Succeeded())
	{
		HeightDataTable = DataTableFinder.Object;
	}

}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
	//FString FilePath = FPaths::ProjectContentDir() / TEXT("/SBS/Source/TileHeightData.csv");

	//FTileHeightData HeightData = ReadHeightDataFromCSV(FilePath);
	//if (HeightData.Height.Num() > 0)
	//{
	//	//UE_LOG(LogTemp, Warning, TEXT("Height Data: %d x %d"), HeightData.Height.Num());
	//}
	 
	GenerateMap(); // 맵 생성

	//Height = HeightDataTable->GetRowNames().Num(); // 행갯수
	//Width = HeightDataTable->GetRowNames().Num(); // 열갯수

	for (int32 Row = 0; Row < Height; Row++)
	{
		for (int32 Col = 0; Col < Width; Col++)
		{
			FVector Location(Row * 100.f, Col * 100.f, TileHeights[Row][Col] * 100.f);  // X, Y, Z 위치 설정
			GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), Location, FRotator::ZeroRotator);  // 기본 액터 스폰
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

void ATile::GenerateTiles(const FTileHeightData& HeightData)
{
	//const int32 NumRows = static_cast<int32>(FMath::Sqrt(static_cast<float>(HeightData.Height.Num())));
	//const int32 NumCols = NumRows;
	//
	//// Iterate through each cell in the height data
	//for (int32 X = 0; X < NumRows; ++X)
	//{
	//	for (int32 Y = 0; Y < NumCols; ++Y)
	//	{
	//		int index = X * NumCols + Y;
	//		float Z = HeightData.Height[index];
	//		FVector Location(X * 100.0f, Y * 100.0f, Z* 100);
	//		FRotator Rotation(0.0f, 0.0f, 0.0f);
	//
	//		// Spawn a new tile actor at the specified location and rotation
	//		GetWorld()->SpawnActor<ATile>(ATile::StaticClass(), Location, Rotation);
	//	}
	//}
}

void ATile::GenerateMap()
{
	if (!HeightDataTable) return;  // 데이터 테이블 없으면 종료

	TArray<FName> RowNames = HeightDataTable->GetRowNames();  // CSV의 모든 행 이름
	Height = RowNames.Num();  // 행 수를 CSV 행 수로 설정
	TileHeights.Empty();  // 기존 데이터 초기화
	TileHeights.SetNum(Height);  // 행 수 설정

	// 첫 번째 행에서 열 수 계산
	FTileHeightData* FirstRow = HeightDataTable->FindRow<FTileHeightData>(RowNames[0], "");
	if (FirstRow)
	{
		TArray<FString> ColData;
		FirstRow->Height.ParseIntoArray(ColData, TEXT(","), true);  // 첫 행의 높이값 분리
		Width = ColData.Num();  // 열 수를 CSV 열 수로 설정
	}

	// 각 행에 데이터 채우기
	for (const FName& RowName : RowNames)
	{
		FTileHeightData* Row = HeightDataTable->FindRow<FTileHeightData>(RowName, "");  // 행 데이터 조회
		if (Row)
		{
			TArray<FString> ColData;
			Row->Height.ParseIntoArray(ColData, TEXT(","), true);  // 높이값 쉼표로 분리
			int32 RowIdx = FCString::Atoi(*RowName.ToString());  // 행 이름 정수로 변환 (예: "0" -> 0)
			if (TileHeights.IsValidIndex(RowIdx))
			{
				TileHeights[RowIdx].SetNum(Width);  // 열 수 설정
				for (int32 Col = 0; Col < ColData.Num() && Col < Width; Col++)
				{
					TileHeights[RowIdx][Col] = FCString::Atoi(*ColData[Col]);  // 높이값 저장
				}
			}
		}
	}

}

