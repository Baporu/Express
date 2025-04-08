// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Exp_GameMode.h"
#include "Tile.generated.h"
USTRUCT() // ����ü ����
struct FTileHeightData : public FTableRowBase // ��� UDatatable�� �� ������ ������ ǥ��ȭ
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Height; // ���̰� ���ڿ�
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

	TArray<TArray<int>> TileHeights; // ���̰� 2d �迭
	int Width;
	int Height;

public:

	FTileHeightData ReadHeightDataFromCSV(const FString& FilePath);
	void GenerateMap();
	
};
