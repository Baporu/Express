// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Exp_GameMode.h"
#include "Tile.generated.h"
USTRUCT() // 구조체 정의
struct FTileHeightData : public FTableRowBase // 상속 UDatatable의 행 데이터 구조를 표준화
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Height; // 높이값 문자열
};
UCLASS()
class EXPRESS_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UStaticMeshComponent* TileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FSM)
	class UTile_FSM* TileFSM;

	UPROPERTY(EditAnywhere, Category ="Data")
	class UDataTable* HeightDataTable;

	TArray<TArray<int>> TileHeights; // 높이값 2d 배열
	int Width;
	int Height;

public:

	FTileHeightData ReadHeightDataFromCSV(const FString& FilePath);
	void GenerateMap();
	
};
