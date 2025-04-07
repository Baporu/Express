// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Exp_GameMode.h"
#include "Tile.generated.h"
USTRUCT() // ±∏¡∂√º ¡§¿«
struct FTileHeightData : public FTableRowBase // ªÛº” UDatatable¿« «‡ µ•¿Ã≈Õ ±∏¡∂∏¶ «•¡ÿ»≠
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Height; // ≥Ù¿Ã∞™ πÆ¿⁄ø≠
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

<<<<<<< HEAD
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UStaticMeshComponent* TileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = FSM)
	class UTile_FSM* TileFSM;

	TArray<TArray<int>> TileHeights; // ≥Ù¿Ã∞™ 2d πËø≠
	
	UPROPERTY(EditAnywhere, Category="Data")
	UDataTable* HeightDataTable;

	int Width;
	int Height;

public:

	FTileHeightData ReadHeightDataFromCSV(const FString& FilePath);
	void GenerateTiles(const FTileHeightData& HeightData);

	void GenerateMap();
	
=======
>>>>>>> parent of ac112c5 (Í≤åÏûÑÎ™®Îìú ÏÉùÏÑ±)
};
